/*
 *  Project  -  gui_cmd
 *  Date     -  June 25th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::types::network::{send_response, Server};
use crate::gui_cmd::format_cmd::format_bct;
use mio::Token;

pub fn cmd_mct(token: Token, server: &mut Server) {
    let mut res = String::new();
    for y in 0..server.params.height {
        for x in 0..server.params.width {
            res.push_str(&format_bct(server, x, y));
        }
    }
    let client = server.clients.get_mut(&token).unwrap();
    let _ = send_response(&mut client.stream, &res);
}
