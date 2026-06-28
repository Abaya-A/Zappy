/*
 *  Project  -  cmd
 *  Date     -  June 25th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::types::network::{notify_gui, send_response, Server};
use mio::Token;

pub fn cmd_unknown(token: Token, server: &mut Server) {
    let client = server.clients.get_mut(&token).unwrap();
    let _ = send_response(&mut client.stream, "ko\n");
    notify_gui(&mut server.clients, "suc\n");
}
