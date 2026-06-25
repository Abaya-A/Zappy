/*
 *  Project  -  gui_cmd
 *  Date     -  June 25th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::utils::Server;
use mio::Token;

pub fn cmd_bct(token: Token, server: &mut Server, x: u32, y: u32) {
    let client = server.clients.get_mut(&token).unwrap();
    let res = format!("bct {} {} 0 0 0 0 0 0 0\n", x, y);
    let _ = crate::utils::send_response(&mut client.stream, &res);
}
