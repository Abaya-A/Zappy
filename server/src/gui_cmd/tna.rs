/*
 *  Project  -  gui_cmd
 *  Date     -  June 25th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::types::network::{send_response, Server};
use mio::Token;

pub fn cmd_tna(token: Token, server: &mut Server) {
    let mut res = String::new();
    for team in &server.params.teams_names {
        res.push_str(&format!("tna {}\n", team));
    }
    let client = server.clients.get_mut(&token).unwrap();
    let _ = send_response(&mut client.stream, &res);
}