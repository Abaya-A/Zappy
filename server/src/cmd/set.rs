/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Server, send_response};

pub fn cmd_set(token: Token, server: &mut Server, obj: String)
{
    let (x, y) = {
        let player = server.clients.get(&token).unwrap().player.as_ref().unwrap();
        (player.x as usize, player.y as usize)
    };

    let client = server.clients.get_mut(&token).unwrap();
    let inv = client.player.as_mut().unwrap().inventory.entry(obj.clone()).or_insert(0);

    if *inv > 0 {
        *inv -= 1;
        server.world.tiles[y][x].resources.entry(obj).and_modify(|v| *v += 1).or_insert(1);
        let client = server.clients.get_mut(&token).unwrap();
        let _ = send_response(&mut client.stream, "ok\n");
    } else {
        let _ = send_response(&mut client.stream, "ko\n");
    }
}
