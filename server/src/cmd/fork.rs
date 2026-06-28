/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Server, send_result, Egg, notify_gui};
use crate::timers;

pub fn cmd_fork(token: Token, server: &mut Server)
{
    if !timers::can_act(token, server) {
        send_result(token, server, "ko");
        return;
    }

    let (x, y, team) = {
        let client = server.clients.get(&token).unwrap();
        let player = client.player.as_ref().unwrap();
        (player.x, player.y, client.team_name.clone().unwrap())
    };

    // recup l'id du nième egg
    let egg_id = server.world.eggs.len() as u32;

    server.world.eggs.push(Egg { team, x, y });

    send_result(token, server, "ok");
    timers::start_action(token, server, 42);

    let n = token.0 as u32;
    notify_gui(&mut server.clients, &format!("pfk #{}\n", n));
    notify_gui(&mut server.clients, &format!("enw #{} #{} {} {}\n", egg_id, n, x, y));
}
