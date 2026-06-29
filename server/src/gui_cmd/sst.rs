/*
 *  Project  -  gui_cmd
 *  Date     -  June 25th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::types::network::{send_response, Server};
use mio::Token;

pub fn cmd_sst(token: Token, server: &mut Server, t: u32) {
    server.params.frequency = t;
    let res = format!("sst {}\n", t);
    let client = server.clients.get_mut(&token).unwrap();
    let _ = send_response(&mut client.stream, &res);
}