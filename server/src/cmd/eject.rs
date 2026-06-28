/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Direction, Server, format_ppo, send_response, notify_gui, compute_direction, send_result};
use crate::timers;

fn get_player_infos(token: Token, server: &Server) -> (u32, u32, Direction)
{
    let client = server.clients.get(&token).unwrap();
    let player = client.player.as_ref().unwrap();

    (player.x, player.y, player.direction.clone())
}

fn next_tile(x: u32, y: u32, direction: &Direction, server: &Server,) -> (u32, u32)
{
    match direction {
        Direction::N => (x, (y + server.params.height - 1) % server.params.height),
        Direction::S => (x, (y + 1) % server.params.height),
        Direction::E => ((x + 1) % server.params.width, y),
        Direction::W => ((x + server.params.width - 1) % server.params.width, y),
    }
}

fn move_players(server: &mut Server, players: &[Token], x: u32, y: u32, new_x: u32, new_y: u32)
{
    for token in players {
        server.world.tiles[y as usize][x as usize].players.retain(|&t| t != *token);

        server.world.tiles[new_y as usize][new_x as usize].players.push(*token);

        let player = server.clients.get_mut(token).unwrap().player.as_mut().unwrap();

        player.x = new_x;
        player.y = new_y;
    }
}

fn notify_players(server: &mut Server, players: &[Token], ejector_x: u32, ejector_y: u32, direction: &Direction)
{
    let width = server.params.width;
    let height = server.params.height;

    for token in players {
        let client = server.clients.get_mut(token).unwrap();
        if let Some(player) = &client.player {
            let k = compute_direction(
                ejector_x, ejector_y,
                player.x, player.y,
                &player.direction,
                width, height,
            );
            let _ = send_response(&mut client.stream, &format!("eject: {}\n", k));
        }
    }
}

pub fn cmd_eject(token: Token, server: &mut Server)
{
    if !timers::can_act(token, server) {
        send_result(token, server, "ko");
        return;
    }

    let (x, y, direction) = get_player_infos(token, server);
    let others: Vec<Token> = server.world.tiles[y as usize][x as usize].players.iter().filter(|&&t| t != token).copied().collect();

    if others.is_empty() {
        send_result(token, server, "ko");
        return;
    }

    let (new_x, new_y) = next_tile(x, y, &direction, server);

    // destroy eggs on the ejected tiles
    server.world.eggs.retain(|egg| !(egg.x == x && egg.y == y));

    move_players(server, &others, x, y, new_x, new_y);
    notify_players(server, &others, x, y, &direction);

    send_result(token, server, "ok");
    timers::start_action(token, server, 7);

    // notify gui
    for other_token in &others {
        let n = other_token.0 as u32;
        let player = server.clients.get(other_token).unwrap().player.as_ref().unwrap();
        let ppo = format_ppo(n, new_x, new_y, player);
        notify_gui(&mut server.clients, &format!("pex #{}\n", n));
        notify_gui(&mut server.clients, &ppo);
    }
}
