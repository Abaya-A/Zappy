/*
 *  Project  -  gui_cmd
 *  Date     -  June 25th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::utils::Server;
use mio::Token;

pub fn cmd_mct(token: Token, server: &mut Server) {
    let client = server.clients.get_mut(&token).unwrap();
    let mut res = String::new();
    for y in 0..server.params.height {
        for x in 0..server.params.width {
            res.push_str(&format!("bct {} {} 0 0 0 0 0 0 0\n", x, y));
        }
    }
    let _ = crate::utils::send_response(&mut client.stream, &res);
}
