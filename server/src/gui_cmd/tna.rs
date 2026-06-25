/*
 *  Project  -  gui_cmd
 *  Date     -  June 25th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::utils::Server;
use mio::Token;

pub fn cmd_tna(token: Token, server: &mut Server) {
    let client = server.clients.get_mut(&token).unwrap();
    let mut res = String::new();
    for team in &server.params.teams_names {
        res.push_str(&format!("tna {}\n", team));
    }
    let _ = crate::utils::send_response(&mut client.stream, &res);
}