/*
 *  Project  -  gui_cmd
 *  Date     -  June 25th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::utils::Server;
use mio::Token;

pub fn cmd_unknown_gui(token: Token, server: &mut Server) {
    let client = server.clients.get_mut(&token).unwrap();
    let _ = crate::utils::send_response(&mut client.stream, "suc\n");
}