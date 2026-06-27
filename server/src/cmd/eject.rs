/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Server, send_response, Direction, send_result};
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

fn notify_players(server: &mut Server, players: &[Token], direction: &Direction)
{
    let dir = match direction {
        Direction::N => Direction::S.to_num(),
        Direction::S => Direction::N.to_num(),
        Direction::E => Direction::W.to_num(),
        Direction::W => Direction::E.to_num(),
    };

    for token in players {
        let client = server.clients.get_mut(token).unwrap();
        let _ = send_response(
            &mut client.stream,
            &format!("eject: {}\n", dir),
        );
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
    move_players(server, &others, x, y, new_x, new_y);
    notify_players(server, &others, &direction);

    send_result(token, server, "ok");
    timers::start_action(token, server, 7);
}