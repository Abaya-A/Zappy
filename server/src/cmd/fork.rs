/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Server, send_response, Egg, notify_gui};

pub fn cmd_fork(token: Token, server: &mut Server)
{
    let (x, y, team) = {
        let client = server.clients.get(&token).unwrap();
        let player = client.player.as_ref().unwrap();
        (player.x, player.y, client.team_name.clone().unwrap())
    };

    // recup l'id du nième egg
    let egg_id = server.world.eggs.len() as u32;

    server.world.eggs.push(Egg { team, x, y });

    let client = server.clients.get_mut(&token).unwrap();
    let _ = send_response(&mut client.stream, "ok\n");

    let n = token.0 as u32;
    notify_gui(&mut server.clients, &format!("pfk #{}\n", n));
    notify_gui(&mut server.clients, &format!("enw #{} #{} {} {}\n", egg_id, n, x, y));
}
