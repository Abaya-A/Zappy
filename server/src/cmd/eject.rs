/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Server, send_response, Direction};

pub fn cmd_eject(token: Token, server: &mut Server)
{
    let (x, y, direction) = {
        let client = server.clients.get(&token).unwrap();
        let player = client.player.as_ref().unwrap();
        (player.x, player.y, player.direction.clone())
    };

    let others: Vec<Token> = server.world.tiles[y as usize][x as usize].players.iter().filter(|&&t| t != token).copied().collect();

    if others.is_empty() {
        let client = server.clients.get_mut(&token).unwrap();
        let _ = send_response(&mut client.stream, "ko\n");
        return;
    }

    let (new_x, new_y) = match direction {
        Direction::N => (x, if y == 0 { server.params.height - 1 } else { y - 1 }),
        Direction::S => (x, (y + 1) % server.params.height),
        Direction::E => ((x + 1) % server.params.width, y),
        Direction::W => (if x == 0 { server.params.width - 1 } else { x - 1 }, y),
    };

    for other_token in &others {
        // maj tile
        server.world.tiles[y as usize][x as usize].players.retain(|&t| t != *other_token);
        server.world.tiles[new_y as usize][new_x as usize].players.push(*other_token);
        // maj player
        let other_player = server.clients.get_mut(other_token).unwrap().player.as_mut().unwrap();
        other_player.x = new_x;
        other_player.y = new_y;
    }

    let dir = match direction {
        Direction::N => Direction::S.to_num(),
        Direction::S => Direction::N.to_num(),
        Direction::E => Direction::W.to_num(),
        Direction::W => Direction::E.to_num(),
    };

    for other_token in &others {
        let client = server.clients.get_mut(other_token).unwrap();
        let _ = send_response(&mut client.stream, &format!("eject: {}\n", dir));
    }

    let client = server.clients.get_mut(&token).unwrap();
    let _ = send_response(&mut client.stream, "ok\n");
}