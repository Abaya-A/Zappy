/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Server, send_response};

pub fn cmd_take(token: Token, server: &mut Server, obj: String)
{
    let (x, y) = {
        let player = server.clients.get(&token).unwrap().player.as_ref().unwrap();
        (player.x as usize, player.y as usize)
    };

    let tile = &mut server.world.tiles[y][x];
    let count = tile.resources.entry(obj.clone()).or_insert(0);

    if *count > 0 {
        *count -= 1;
        let client = server.clients.get_mut(&token).unwrap();
        *client.player.as_mut().unwrap().inventory.entry(obj).or_insert(0) += 1;
        let _ = send_response(&mut client.stream, "ok\n");
    } else {
        let client = server.clients.get_mut(&token).unwrap();
        let _ = send_response(&mut client.stream, "ko\n");
    }
}
