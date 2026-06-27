/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin & Léo Lacordaire
 */

use mio::Token;
use crate::utils::{Direction, Server, send_result};
use crate::timers;

fn get_movement(token: Token, server: &Server) -> (u32, u32, u32, u32)
{
    let client = server.clients.get(&token).unwrap();
    let player = client.player.as_ref().unwrap();

    let ox = player.x;
    let oy = player.y;

    let (nx, ny) = match player.direction {
        Direction::N => (ox, if oy == 0 { server.params.height - 1 } else { oy - 1 }),
        Direction::S => (ox, (oy + 1) % server.params.height),
        Direction::E => ((ox + 1) % server.params.width, oy),
        Direction::W => (if ox == 0 { server.params.width - 1 } else { ox - 1 }, oy),
    };

    (ox, oy, nx, ny)
}

fn update_player_position(token: Token, server: &mut Server, x: u32, y: u32)
{
    let client = server.clients.get_mut(&token).unwrap();
    let player = client.player.as_mut().unwrap();

    player.x = x;
    player.y = y;
}

pub fn cmd_forward(token: Token, server: &mut Server)
{
    if !timers::can_act(token, server) {
        send_result(token, server, "ko");
        return;
    }

    let (old_x, old_y, new_x, new_y) = get_movement(token, server);

    server.world.tiles[old_y as usize][old_x as usize].players.retain(|&t| t != token);
    server.world.tiles[new_y as usize][new_x as usize].players.push(token);

    update_player_position(token, server, new_x, new_y);

    send_result(token, server, "ok");
    timers::start_action(token, server, 7);
}