/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin & Léo Lacordaire
 */

use mio::Token;
use crate::utils::{Server, send_response, Direction, Player, notify_gui, format_ppo};

fn go_left(player: &mut Player)
{
    player.direction = match player.direction {
        Direction::N => Direction::W,
        Direction::W => Direction::S,
        Direction::S => Direction::E,
        Direction::E => Direction::N,
    };
}

pub fn cmd_left(token: Token, server: &mut Server)
{
    let client = server.clients.get_mut(&token).unwrap();
    let player = client.player.as_mut().unwrap();

    go_left(player);

    let n = token.0 as u32;
    let _ = send_response(&mut client.stream, "ok\n");

    let ppo = format_ppo(n, player.x, player.y, player);
    notify_gui(&mut server.clients, &ppo);
}
