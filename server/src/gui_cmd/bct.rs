/*
 *  Project  -  gui_cmd
 *  Date     -  June 25th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::types::network::Server;
use crate::types::network::send_response;
use crate::gui_cmd::format_cmd::format_bct;
use mio::Token;

pub fn cmd_bct(token: Token, server: &mut Server, x: u32, y: u32) {
    let res = format_bct(server, x, y);
    let client = server.clients.get_mut(&token).unwrap();
    let _ = send_response(&mut client.stream, &res);
}
