/*
 *  Project  -  server
 *  Date     -  June 28th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::utils;
use crate::utils::Server;
use mio::Token;
use std::io::Read;

pub fn handle_requests(token: Token, server: &mut Server) -> Option<Vec<String>>
{
    let mut requests = Vec::new();
    let mut closed = false;

    let client = match server.clients.get_mut(&token) {
        Some(c) => c,
        None => return None,
    };
    let mut buf = [0u8; 1024];

    match client.stream.read(&mut buf) {
        Ok(0) => {
            closed = true;
        }
        Ok(n) => {
            let data = String::from_utf8_lossy(&buf[..n]);
            client.buffer.push_str(&data);
        }
        Err(_) => {
            closed = true;
        }
    }

    if !closed {
        while let Some(pos) = client.buffer.find('\n') {
            let req = client.buffer[..pos].trim_end_matches('\r').to_string();
            client.buffer = client.buffer[pos + 1..].to_string();
            requests.push(req);
        }
    } else {
        cleanup_client(token, server);
        return None;
    }

    return Some(requests);
}

fn cleanup_client(token: Token, server: &mut Server) {
    if let Some(removed_client) = server.clients.remove(&token) {
        if let Some(player) = &removed_client.player {
            let px = player.x as usize;
            let py = player.y as usize;
            if py < server.world.tiles.len() && px < server.world.tiles[py].len() {
                server.world.tiles[py][px].players.retain(|&t| t != token);
            }
            let death_msg = format!("pdi #{}\n", token.0);
            utils::notify_gui(&mut server.clients, &death_msg);
        }
    }
}
