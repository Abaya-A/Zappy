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

fn go_right(player: &mut Player)
{
    player.direction = match player.direction {
        Direction::N => Direction::E,
        Direction::E => Direction::S,
        Direction::S => Direction::W,
        Direction::W => Direction::N,
    };
}

fn rotate_player_right(token: Token, server: &mut Server) -> String
{
    let client = server.clients.get_mut(&token).unwrap();
    let player = client.player.as_mut().unwrap();

    go_right(player);

    let player_number = token.0 as u32;

    format_ppo(player_number, player.x, player.y, player)
}

pub fn cmd_right(token: Token, server: &mut Server)
{
    let ppo = rotate_player_right(token, server);

    send_result(token, server, "ok");
    notify_gui(&mut server.clients, &ppo);
}