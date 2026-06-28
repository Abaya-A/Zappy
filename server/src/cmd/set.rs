/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Server, send_result, notify_gui, resource_to_index};
use crate::timers;

fn remove_from_inventory(server: &mut Server, token: Token, resource: &str) -> bool
{
    let client = server.clients.get_mut(&token).unwrap();
    let player = client.player.as_mut().unwrap();
    
    if resource == "food" {
        if player.food == 0 { return false; }
        player.food -= 1;
        return true;
    }
    
    let count = player.inventory.entry(resource.to_string()).or_insert(0);

    if *count == 0 { return false; }
    *count -= 1;
    true
}

fn drop_resource(server: &mut Server, x: usize, y: usize, resource: &str)
{
    server.world.tiles[y][x].resources.entry(resource.to_string()).and_modify(|v| *v += 1).or_insert(1);
}

fn notify_set_to_gui(token: Token, server: &mut Server, obj: String)
{
    let i = resource_to_index(&obj);
    let n = token.0 as u32;
    notify_gui(&mut server.clients, &format!("pdr #{} {}\n", n, i));
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
        notify_set_to_gui(token, server, obj);
        timers::start_action(token, server, 7);
    } else {
        send_result(token, server, "ko");
    }
}
