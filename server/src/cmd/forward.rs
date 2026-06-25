/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin & Léo Lacordaire
 */

use mio::Token;
use crate::utils::{Server, send_response, Direction};

pub fn cmd_forward(token: Token, server: &mut Server)
{
    let client = server.clients.get_mut(&token).unwrap();
    let player = client.player.as_mut().unwrap();

    match player.direction {
        Direction::N => {
            if player.y == 0 {
                player.y = server.params.height - 1;
            } else {
                player.y -= 1;
            }
        }
        Direction::S => {
            player.y = (player.y + 1) % server.params.height;
        }
        Direction::E => {
            player.x = (player.x + 1) % server.params.width;
        }
        Direction::W => {
            if player.x == 0 {
                player.x = server.params.width - 1;
            } else {
                player.x -= 1;
            }
        }
    }

    let _ = send_response(&mut client.stream, "ok\n");
}
