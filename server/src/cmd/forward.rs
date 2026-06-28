/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin & Léo Lacordaire
 */

use mio::Token;

use crate::utils::{
    format_ppo,
    notify_gui,
    send_result,
    Direction,
    Server,
};

fn get_movement(token: Token, server: &Server) -> (u32, u32, u32, u32)
{
    let client = server.clients.get(&token).unwrap();
    let player = client.player.as_ref().unwrap();

    let old_x = player.x;
    let old_y = player.y;

    let (new_x, new_y) = match player.direction {
        Direction::N => (
            old_x,
            if old_y == 0 {
                server.params.height - 1
            } else {
                old_y - 1
            },
        ),
        Direction::S => (old_x, (old_y + 1) % server.params.height),
        Direction::E => ((old_x + 1) % server.params.width, old_y),
        Direction::W => (
            if old_x == 0 {
                server.params.width - 1
            } else {
                old_x - 1
            },
            old_y,
        ),
    };

    (old_x, old_y, new_x, new_y)
}

fn move_player_on_tiles(
    token: Token,
    server: &mut Server,
    old_x: u32,
    old_y: u32,
    new_x: u32,
    new_y: u32,
)
{
    server.world.tiles[old_y as usize][old_x as usize]
        .players
        .retain(|&player_token| player_token != token);

    server.world.tiles[new_y as usize][new_x as usize]
        .players
        .push(token);
}

fn update_player_position(token: Token, server: &mut Server, x: u32, y: u32)
{
    let client = server.clients.get_mut(&token).unwrap();
    let player = client.player.as_mut().unwrap();

    player.x = x;
    player.y = y;
}

fn notify_forward_to_gui(token: Token, server: &mut Server)
{
    let player_number = token.0 as u32;
    let ppo = {
        let client = server.clients.get(&token).unwrap();
        let player = client.player.as_ref().unwrap();

        format_ppo(player_number, player.x, player.y, player)
    };

    notify_gui(&mut server.clients, &ppo);
}

pub fn cmd_forward(token: Token, server: &mut Server)
{
    let (old_x, old_y, new_x, new_y) = get_movement(token, server);

    move_player_on_tiles(token, server, old_x, old_y, new_x, new_y);
    update_player_position(token, server, new_x, new_y);

    send_result(token, server, "ok");
    notify_forward_to_gui(token, server);
}