/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;

use crate::utils::{
    compute_direction,
    format_ppo,
    notify_gui,
    send_response,
    send_result,
    Direction,
    Server,
};

fn get_player_infos(token: Token, server: &Server) -> (u32, u32, Direction)
{
    let client = server.clients.get(&token).unwrap();
    let player = client.player.as_ref().unwrap();

    (player.x, player.y, player.direction.clone())
}

fn next_tile(x: u32, y: u32, direction: &Direction, server: &Server) -> (u32, u32)
{
    match direction {
        Direction::N => (x, (y + server.params.height - 1) % server.params.height),
        Direction::S => (x, (y + 1) % server.params.height),
        Direction::E => ((x + 1) % server.params.width, y),
        Direction::W => ((x + server.params.width - 1) % server.params.width, y),
    }
}

fn get_other_players_on_tile(token: Token, server: &Server, x: u32, y: u32) -> Vec<Token>
{
    server.world.tiles[y as usize][x as usize]
        .players
        .iter()
        .filter(|&&player_token| player_token != token)
        .copied()
        .collect()
}

fn destroy_eggs_on_tile(server: &mut Server, x: u32, y: u32)
{
    server.world.eggs.retain(|egg| !(egg.x == x && egg.y == y));
}

fn move_players(
    server: &mut Server,
    players: &[Token],
    old_x: u32,
    old_y: u32,
    new_x: u32,
    new_y: u32,
)
{
    for token in players {
        server.world.tiles[old_y as usize][old_x as usize]
            .players
            .retain(|&player_token| player_token != *token);

        server.world.tiles[new_y as usize][new_x as usize]
            .players
            .push(*token);

        let player = server
            .clients
            .get_mut(token)
            .unwrap()
            .player
            .as_mut()
            .unwrap();

        player.x = new_x;
        player.y = new_y;
    }
}

fn notify_ejected_players(
    server: &mut Server,
    players: &[Token],
    ejector_x: u32,
    ejector_y: u32,
)
{
    let width = server.params.width;
    let height = server.params.height;

    for token in players {
        let client = server.clients.get_mut(token).unwrap();

        let Some(player) = &client.player else {
            continue;
        };

        let direction = compute_direction(
            ejector_x,
            ejector_y,
            player.x,
            player.y,
            &player.direction,
            width,
            height,
        );

        let _ = send_response(&mut client.stream, &format!("eject: {}\n", direction));
    }
}

fn notify_eject_to_gui(
    ejector: Token,
    server: &mut Server,
    ejected_players: &[Token],
)
{
    let ejector_number = ejector.0 as u32;

    notify_gui(&mut server.clients, &format!("pex #{}\n", ejector_number));

    for token in ejected_players {
        let player_number = token.0 as u32;
        let ppo = {
            let client = server.clients.get(token).unwrap();
            let player = client.player.as_ref().unwrap();

            format_ppo(player_number, player.x, player.y, player)
        };

        notify_gui(&mut server.clients, &ppo);
    }
}

pub fn cmd_eject(token: Token, server: &mut Server)
{
    let (x, y, direction) = get_player_infos(token, server);
    let ejected_players = get_other_players_on_tile(token, server, x, y);

    if ejected_players.is_empty() {
        send_result(token, server, "ko");
        return;
    }

    let (new_x, new_y) = next_tile(x, y, &direction, server);

    destroy_eggs_on_tile(server, x, y);
    move_players(server, &ejected_players, x, y, new_x, new_y);
    notify_ejected_players(server, &ejected_players, x, y);

    send_result(token, server, "ok");
    notify_eject_to_gui(token, server, &ejected_players);
}