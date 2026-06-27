/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Server, send_result};
use crate::timers;

fn remove_from_inventory(server: &mut Server, token: Token, resource: &str) -> bool
{
    let client = server.clients.get_mut(&token).unwrap();

    let count = client.player.as_mut().unwrap().inventory.entry(resource.to_string()).or_insert(0);

    if *count == 0 {
        return false;
    }

    *count -= 1;
    true
}

fn drop_resource(server: &mut Server, x: usize, y: usize, resource: &str)
{
    server.world.tiles[y][x].resources.entry(resource.to_string()).and_modify(|v| *v += 1).or_insert(1);
}

pub fn cmd_set(token: Token, server: &mut Server, obj: String)
{
    if !timers::can_act(token, server) {
        send_result(token, server, "ko");
        return;
    }

    let (x, y) = {
        let player = server.clients.get(&token).unwrap().player.as_ref().unwrap();
        (player.x as usize, player.y as usize)
    };

    if remove_from_inventory(server, token, &obj) {
        drop_resource(server, x, y, &obj);
        send_result(token, server, "ok");
        timers::start_action(token, server, 7);
    } else {
        send_result(token, server, "ko");
    }
}
