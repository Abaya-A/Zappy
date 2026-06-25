/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin & Léo Lacordaire
 */

use mio::Token;
use crate::utils::{Server, send_response, Direction};

pub fn cmd_right(token: Token, server: &mut Server)
{
    let client = server.clients.get_mut(&token).unwrap();
    let player = client.player.as_mut().unwrap();

    player.direction = match player.direction {
        Direction::N => Direction::E,
        Direction::E => Direction::S,
        Direction::S => Direction::W,
        Direction::W => Direction::N,
    };

    let _ = send_response(&mut client.stream, "ok\n");
}
