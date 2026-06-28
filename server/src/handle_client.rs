/*
 *  Project  -  src
 *  Date     -  June 9th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;

use crate::handle_requests::handle_requests;
use crate::utils;
use crate::utils::Server;

const DEBUG_COMMAND_QUEUE: bool = true;

fn debug_queue_received(token: Token, is_gui: bool, queue_len: usize, request: &str)
{
    if !DEBUG_COMMAND_QUEUE {
        return;
    }

    println!(
        "[QUEUE] token={:?} is_gui={} len={} cmd={}",
        token,
        is_gui,
        queue_len,
        request
    );
}

fn debug_queue_drop(token: Token, request: &str)
{
    if !DEBUG_COMMAND_QUEUE {
        return;
    }

    println!(
        "[QUEUE DROP] token={:?} cmd={}",
        token,
        request
    );
}

fn debug_handshake(token: Token, team: &str)
{
    if !DEBUG_COMMAND_QUEUE {
        return;
    }

    println!(
        "[HANDSHAKE] token={:?} team={}",
        token,
        team
    );
}

fn queue_authenticated_request(token: Token, server: &mut Server, request: String)
{
    let Some(client) = server.clients.get_mut(&token) else {
        return;
    };

    debug_queue_received(
        token,
        client.is_gui,
        client.command_queue.len(),
        &request,
    );

    if client.is_gui || client.command_queue.len() < 10 {
        client.command_queue.push_back(request);
        return;
    }

    debug_queue_drop(token, &request);
}

pub fn handle_client(token: Token, server: &mut Server) -> Result<(), std::io::Error>
{
    let requests = match handle_requests(token, server) {
        Some(requests) => requests,
        None => return Ok(()),
    };

    for request in requests {
        let is_authenticated = match server.clients.get(&token) {
            Some(client) => client.team_name.is_some(),
            None => continue,
        };

        if !is_authenticated {
            handle_handshake(token, server, request);
            continue;
        }

        queue_authenticated_request(token, server, request);
    }

    Ok(())
}

fn handle_handshake(token: Token, server: &mut Server, team: String)
{
    debug_handshake(token, &team);

    let valid_team = server.params.teams_names.contains(&team) || team == "GRAPHIC";

    if !valid_team {
        let client = server.clients.get_mut(&token).unwrap();
        let _ = utils::send_response(&mut client.stream, "ko\n");
        return;
    }

    if team == "GRAPHIC" {
        register_gui_client(token, server, team);
        return;
    }

    register_ai_client(token, server, team);
}

fn register_gui_client(token: Token, server: &mut Server, team: String)
{
    let existing_players = build_existing_players_message(token, server);

    let client = server.clients.get_mut(&token).unwrap();
    client.team_name = Some(team);
    client.is_gui = true;

    if !existing_players.is_empty() {
        let _ = utils::send_response(&mut client.stream, &existing_players);
    }
}

fn build_existing_players_message(token: Token, server: &Server) -> String
{
    let mut existing_players = String::new();

    for (other_token, client) in &server.clients {
        if *other_token == token {
            continue;
        }

        let (Some(player), Some(team_name)) = (&client.player, &client.team_name) else {
            continue;
        };

        existing_players.push_str(&format!(
            "pnw #{} {} {} {} {} {}\n",
            other_token.0,
            player.x,
            player.y,
            player.direction.to_num(),
            player.level,
            team_name
        ));
    }

    existing_players
}

fn register_ai_client(token: Token, server: &mut Server, team: String)
{
    if !consume_team_slot(server, &team) {
        let client = server.clients.get_mut(&token).unwrap();
        let _ = utils::send_response(&mut client.stream, "ko\n");
        return;
    }

    let (total_available_slots, spawn_x, spawn_y) =
        init_ai_client(token, server, team.clone());

    let response = format!(
        "{}\n{} {}\n",
        total_available_slots,
        server.params.width,
        server.params.height
    );

    let client = server.clients.get_mut(&token).unwrap();
    let _ = utils::send_response(&mut client.stream, &response);

    let pnw_message = format!(
        "pnw #{} {} {} {} 1 {}\n",
        token.0,
        spawn_x,
        spawn_y,
        utils::Direction::N.to_num(),
        team
    );

    utils::notify_gui(&mut server.clients, &pnw_message);
}

fn consume_team_slot(server: &mut Server, team: &str) -> bool
{
    let Some(team_data) = server.teams.iter_mut().find(|team_data| team_data.name == team) else {
        return false;
    };

    if team_data.available_slots == 0 {
        return false;
    }

    team_data.available_slots -= 1;
    true
}

fn init_ai_client(token: Token, server: &mut Server, team: String) -> (usize, u32, u32)
{
    let (spawn_x, spawn_y) = get_spawn_position(server, &team);
    let total_available_slots = server
        .teams
        .iter()
        .find(|team_data| team_data.name == team)
        .map(|team_data| team_data.available_slots)
        .unwrap_or(0);

    let client = server.clients.get_mut(&token).unwrap();

    client.team_name = Some(team);
    client.player = Some(utils::Player {
        x: spawn_x,
        y: spawn_y,
        direction: utils::Direction::N,
        level: 1,
        food: 10,
        inventory: std::collections::HashMap::new(),
        last_food_update: std::time::SystemTime::now(),
    });

    server.world.tiles[spawn_y as usize][spawn_x as usize]
        .players
        .push(token);

    (total_available_slots, spawn_x, spawn_y)
}

fn get_spawn_position(server: &mut Server, team: &str) -> (u32, u32)
{
    let egg_position = server.world.eggs.iter().position(|egg| egg.team == team);

    if let Some(index) = egg_position {
        let egg = server.world.eggs.remove(index);
        return (egg.x, egg.y);
    }

    let seed = std::time::SystemTime::now()
        .duration_since(std::time::SystemTime::UNIX_EPOCH)
        .unwrap()
        .subsec_nanos();

    (
        seed % server.params.width,
        (seed / server.params.width) % server.params.height,
    )
}