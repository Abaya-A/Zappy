/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Server, send_response, Egg};

pub fn cmd_fork(token: Token, server: &mut Server)
{
    let (x, y, team) = {
        let client = server.clients.get(&token).unwrap();
        let player = client.player.as_ref().unwrap();
        (player.x, player.y, client.team_name.clone().unwrap())
    };

    server.world.eggs.push(Egg { team, x, y });

    let client = server.clients.get_mut(&token).unwrap();
    let _ = send_response(&mut client.stream, "ok\n");
}
