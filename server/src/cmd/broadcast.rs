/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::Server;

pub fn cmd_broadcast(token: Token, server: &mut Server, _msg: String)
{
    for (other_token, client) in server.clients.iter_mut() {
        if *other_token == token {
            continue;
        }

        let msg = format!("message 0, {}\n", _msg);
        let _ = crate::utils::send_response(
            &mut client.stream,
            &msg
        );
    }

    let sender = server.clients.get_mut(&token).unwrap();
    let _ = crate::utils::send_response(
        &mut sender.stream,
        "ok\n"
    );
}
