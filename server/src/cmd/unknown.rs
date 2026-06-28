/*
 *  Project  -  cmd
 *  Date     -  June 25th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::utils::Server;
use mio::Token;
use crate::utils::notify_gui;

pub fn cmd_unknown(token: Token, server: &mut Server) {
    let client = server.clients.get_mut(&token).unwrap();
    let _ = crate::utils::send_response(&mut client.stream, "ko\n");
    notify_gui(&mut server.clients, "suc\n");
}
