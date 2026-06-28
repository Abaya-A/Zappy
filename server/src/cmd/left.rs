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
    Player,
    Server,
};

fn go_left(player: &mut Player)
{
    player.direction = match player.direction {
        Direction::N => Direction::W,
        Direction::W => Direction::S,
        Direction::S => Direction::E,
        Direction::E => Direction::N,
    };
}

fn rotate_player_left(token: Token, server: &mut Server) -> String
{
    let client = server.clients.get_mut(&token).unwrap();
    let player = client.player.as_mut().unwrap();

    go_left(player);

    let player_number = token.0 as u32;

    format_ppo(player_number, player.x, player.y, player)
}

pub fn cmd_left(token: Token, server: &mut Server)
{
    let ppo = rotate_player_left(token, server);

    send_result(token, server, "ok");
    notify_gui(&mut server.clients, &ppo);
}