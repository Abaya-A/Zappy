/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin & Léo Lacordaire
 */

use mio::Token;
use crate::utils::{Server, Player, Direction, send_result};
use crate::timers;

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
    if !timers::can_act(token, server) {
        send_result(token, server, "ko");
        return;
    }

    let client = server.clients.get_mut(&token).unwrap();
    let player = client.player.as_mut().unwrap();

    go_right(player);

    send_result(token, server, "ok");
    timers::start_action(token, server, 7);
}
