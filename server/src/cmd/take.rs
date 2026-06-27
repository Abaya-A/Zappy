/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Server, send_result};
use crate::timers;

fn take_resource(server: &mut Server, x: usize, y: usize, resource: &str) -> bool
{
    let tile = &mut server.world.tiles[y][x];
    let count = tile.resources.entry(resource.to_string()).or_insert(0);

    if *count == 0 {
        return false;
    }

    *count -= 1;
    true
}

fn add_to_inventory(server: &mut Server, token: Token, resource: &str)
{
    let client = server.clients.get_mut(&token).unwrap();

    *client.player.as_mut().unwrap().inventory.entry(resource.to_string()).or_insert(0) += 1;
}

pub fn cmd_take(token: Token, server: &mut Server, obj: String)
{
    if !timers::can_act(token, server) {
        send_result(token, server, "ko");
        return;
    }

    let (x, y) = {
        let player = server.clients.get(&token).unwrap().player.as_ref().unwrap();
        (player.x as usize, player.y as usize)
    };

    if take_resource(server, x, y, &obj) {
        add_to_inventory(server, token, &obj);
        send_result(token, server, "ok");
        timers::start_action(token, server, 7);
    } else {
        send_result(token, server, "ko");
    }
}
