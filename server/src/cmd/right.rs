/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin & Léo Lacordaire
 */

use mio::Token;
use crate::utils::{Direction, Player, Server, format_ppo, notify_gui, send_response};

fn go_right(player: &mut Player)
{
    player.direction = match player.direction {
        Direction::N => Direction::E,
        Direction::E => Direction::S,
        Direction::S => Direction::W,
        Direction::W => Direction::N,
    };
}

pub fn cmd_right(token: Token, server: &mut Server)
{
    let client = server.clients.get_mut(&token).unwrap();
    let player = client.player.as_mut().unwrap();

    go_right(player);

    let n = token.0 as u32;
    let _ = send_response(&mut client.stream, "ok\n");

    let ppo = format_ppo(n, player.x, player.y, player);
    notify_gui(&mut server.clients, &ppo);
}
