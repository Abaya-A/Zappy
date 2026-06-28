/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;

use crate::utils::{
    notify_gui,
    send_result,
    Player,
    Server,
};

use std::collections::HashMap;

struct IncantationRequirements {
    players: usize,
    resources: HashMap<String, u32>,
}

fn get_requirements(level: u32) -> Option<IncantationRequirements>
{
    let mut resources = HashMap::new();

    let (players, requirements) = match level {
        1 => (1, vec![("linemate", 1)]),
        2 => (2, vec![("linemate", 1), ("deraumere", 1), ("sibur", 1)]),
        3 => (2, vec![("linemate", 2), ("sibur", 1), ("phiras", 2)]),
        4 => (4, vec![("linemate", 1), ("deraumere", 1), ("sibur", 2), ("phiras", 1)]),
        5 => (4, vec![("linemate", 1), ("deraumere", 2), ("sibur", 1), ("mendiane", 3)]),
        6 => (6, vec![("linemate", 1), ("deraumere", 2), ("sibur", 3), ("phiras", 1)]),
        7 => (6, vec![
            ("linemate", 2),
            ("deraumere", 2),
            ("sibur", 2),
            ("mendiane", 2),
            ("phiras", 2),
            ("thystame", 1),
        ]),
        _ => return None,
    };

    for (resource, count) in requirements {
        resources.insert(resource.to_string(), count);
    }

    Some(IncantationRequirements { players, resources })
}

fn get_player_snapshot(token: Token, server: &Server) -> Option<Player>
{
    server
        .clients
        .get(&token)
        .and_then(|client| client.player.clone())
}

fn get_tile_players_at_same_level(server: &Server, player: &Player) -> Vec<Token>
{
    server.world.tiles[player.y as usize][player.x as usize]
        .players
        .iter()
        .copied()
        .filter(|token| {
            server
                .clients
                .get(token)
                .and_then(|client| client.player.as_ref())
                .map(|other| other.level == player.level)
                .unwrap_or(false)
        })
        .collect()
}

fn has_required_players(
    server: &Server,
    player: &Player,
    requirements: &IncantationRequirements,
) -> bool {
    get_tile_players_at_same_level(server, player).len() >= requirements.players
}

fn has_required_resources(
    server: &Server,
    player: &Player,
    requirements: &IncantationRequirements,
) -> bool {
    let tile = &server.world.tiles[player.y as usize][player.x as usize];

    requirements.resources.iter().all(|(resource, required_count)| {
        tile.resources.get(resource).copied().unwrap_or(0) >= *required_count
    })
}

fn check_incantation(server: &Server, player: &Player) -> bool
{
    let Some(requirements) = get_requirements(player.level) else {
        return false;
    };

    has_required_players(server, player, &requirements)
        && has_required_resources(server, player, &requirements)
}

fn remove_required_resources(server: &mut Server, player: &Player)
{
    let Some(requirements) = get_requirements(player.level) else {
        return;
    };

    let tile = &mut server.world.tiles[player.y as usize][player.x as usize];

    for (resource, required_count) in requirements.resources {
        let count = tile.resources.entry(resource).or_insert(0);
        *count = count.saturating_sub(required_count);
    }
}

fn level_up_participants(server: &mut Server, player: &Player)
{
    let participants = get_tile_players_at_same_level(server, player);

    for token in participants {
        let Some(client) = server.clients.get_mut(&token) else {
            continue;
        };

        let Some(participant) = client.player.as_mut() else {
            continue;
        };

        participant.level += 1;
    }
}

fn build_pic_message(player: &Player, participants: &[Token]) -> String
{
    let mut message = format!("pic {} {} {}", player.x, player.y, player.level);

    for token in participants {
        message.push_str(&format!(" #{}", token.0));
    }

    message.push('\n');
    message
}

fn notify_incantation_start(server: &mut Server, player: &Player)
{
    let participants = get_tile_players_at_same_level(server, player);
    let message = build_pic_message(player, &participants);

    notify_gui(&mut server.clients, &message);
}

fn notify_incantation_result(server: &mut Server, player: &Player, success: bool)
{
    let result = if success { 1 } else { 0 };
    let message = format!("pie {} {} {}\n", player.x, player.y, result);

    notify_gui(&mut server.clients, &message);
}

fn incantation_failed(token: Token, server: &mut Server, player: &Player)
{
    send_result(token, server, "ko");
    notify_incantation_result(server, player, false);
}

fn incantation_succeeded(token: Token, server: &mut Server, player: &Player)
{
    remove_required_resources(server, player);
    level_up_participants(server, player);

    let current_level = server
        .clients
        .get(&token)
        .and_then(|client| client.player.as_ref())
        .map(|participant| participant.level)
        .unwrap_or(player.level + 1);

    send_result(token, server, &format!("Current level: {}", current_level));
    notify_incantation_result(server, player, true);
}

pub fn cmd_incantation(token: Token, server: &mut Server)
{
    let Some(player) = get_player_snapshot(token, server) else {
        send_result(token, server, "ko");
        return;
    };

    notify_incantation_start(server, &player);

    if !check_incantation(server, &player) {
        incantation_failed(token, server, &player);
        return;
    }

    incantation_succeeded(token, server, &player);
}