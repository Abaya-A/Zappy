/*
 *  Project  -  gui_cmd
 *  Date     -  June 25th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::types::network::{send_response, Server};
use mio::Token;

pub fn cmd_sgt(token: Token, server: &mut Server) {
    let client = server.clients.get_mut(&token).unwrap();
    let res = format!("sgt {}\n", server.params.frequency);
    let _ = send_response(&mut client.stream, &res);
}
