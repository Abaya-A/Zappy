/*
 *  Project  -  gui_cmd
 *  Date     -  June 25th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::utils::Server;
use mio::Token;

pub fn cmd_ppo(token: Token, server: &mut Server, n: u32) {
    let client = server.clients.get_mut(&token).unwrap();
    let res = format!("ppo {} 0 0 1\n", n);
    let _ = crate::utils::send_response(&mut client.stream, &res);
}
