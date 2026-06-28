/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Server, send_result, notify_gui, resource_to_index};
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
    let player = client.player.as_mut().unwrap();

    if resource == "food" {
        player.food += 1;
        return;
    }

    *player.inventory.entry(resource.to_string()).or_insert(0) += 1;
}

pub fn cmd_take(token: Token, server: &mut Server, obj: String)
{
    if !timers::can_act(token, server) {
        send_result(token, server, "ko");
        return;
    }

    let (x, y, n) = {
        let player = server.clients.get(&token).unwrap().player.as_ref().unwrap();
        (player.x as usize, player.y as usize, token.0 as u32)
    };

    if take_resource(server, x, y, &obj) {
        add_to_inventory(server, token, &obj);
        send_result(token, server, "ok");
        let i = resource_to_index(&obj);
        notify_gui(&mut server.clients, &format!("pgt #{} {}\n", n, i));
        timers::start_action(token, server, 7);
    } else {
        send_result(token, server, "ko");
    }
}
