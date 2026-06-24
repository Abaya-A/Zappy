/*
 *  Project  -  cmd
 *  Date     -  June 10th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin & Léo Lacordaire
 */

use mio::Token;
use crate::utils;
use crate::utils::Server;

pub fn cmd_forward(token: Token, server: &mut Server)
{
    let width = server.params.width;
    let client = server.clients.get_mut(&token).unwrap();
    let player = client.player.as_mut().unwrap();

    player.x = (player.x + 1) % width;

    let _ = utils::send_response(&mut client.stream, "ok\n");
}
