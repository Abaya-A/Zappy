/*
 *  Project  -  gui_cmd
 *  Date     -  June 25th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::types::network::{send_response, Server};
use crate::gui_cmd::format_cmd::format_pin;
use mio::Token;

pub fn cmd_pin(token: Token, server: &mut Server, n: u32) {
    let mut found = None;
    for (t, c) in &server.clients {
        if t.0 as u32 == n {
            if let Some(player) = &c.player {
                found = Some(format_pin(n, player));
            }
            break;
        }
    }

    let response = match found {
        Some(res) => res,
        None => "sbp\n".to_string(),
    };

    let client = server.clients.get_mut(&token).unwrap();
    let _ = send_response(&mut client.stream, &response);
}
