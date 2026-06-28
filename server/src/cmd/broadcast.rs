/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::timers;
use crate::utils::{Server, send_result, notify_gui, compute_direction};

fn broadcast_to_others(token: Token, server: &mut Server, msg: &str, ex: u32, ey: u32)
{
    let width = server.params.width;
    let height = server.params.height;

    for (other_token, client) in server.clients.iter_mut() {
        if *other_token == token || client.is_gui {
            continue;
        }
        if let Some(player) = &client.player {
            let k = compute_direction(ex, ey, player.x, player.y, &player.direction, width, height);
            let _ = crate::utils::send_response(
                &mut client.stream,
                &format!("message {}, {}\n", k, msg),
            );
        }
    }
}

pub fn cmd_broadcast(token: Token, server: &mut Server, msg: String)
{
    if !timers::can_act(token, server) {
        send_result(token, server, "ko");
        return;
    }
    
    let (n, ex, ey) = {
        let client = server.clients.get(&token).unwrap();
        let player = client.player.as_ref().unwrap();
        (token.0 as u32, player.x, player.y)
    };

    broadcast_to_others(token, server, &msg, ex, ey);
    send_result(token, server, "ok");
    timers::start_action(token, server, 7);
    notify_gui(&mut server.clients, &format!("pbc #{} {}\n", n, msg));
}
