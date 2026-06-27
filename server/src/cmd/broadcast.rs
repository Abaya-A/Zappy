/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Server, send_result};
use crate::timers;

fn broadcast_to_others(token: Token, server: &mut Server, msg: &str)
{
    for (other_token, client) in server.clients.iter_mut() {
        if *other_token == token {
            continue;
        }

        let _ = crate::utils::send_response(
            &mut client.stream,
            msg
        );
    }
}

pub fn cmd_broadcast(token: Token, server: &mut Server, msg: String)
{
    if !timers::can_act(token, server) {
        send_result(token, server, "ko");
        return;
    }

    let formatted = format!("message 0, {}\n", msg);

    broadcast_to_others(token, server, &formatted);
    send_result(token, server, "ok");
    timers::start_action(token, server, 7);
}