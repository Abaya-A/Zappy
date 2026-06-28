/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;

use crate::utils::{notify_gui, resource_to_index, send_result, Server};

fn remove_from_inventory(server: &mut Server, token: Token, resource: &str) -> bool
{
    let client = server.clients.get_mut(&token).unwrap();
    let player = client.player.as_mut().unwrap();

    if resource == "food" {
        if player.food == 0 {
            return false;
        }
        player.food -= 1;
        return true;
    }

    let count = player.inventory.entry(resource.to_string()).or_insert(0);

    if *count == 0 {
        return false;
    }

    *count -= 1;
    true
}

fn drop_resource(server: &mut Server, x: usize, y: usize, resource: &str)
{
    *server.world.tiles[y][x]
        .resources
        .entry(resource.to_string())
        .or_insert(0) += 1;
}

fn notify_set_to_gui(token: Token, server: &mut Server, resource: &str)
{
    let player_number = token.0 as u32;
    let resource_index = resource_to_index(resource);
    let message = format!("pdr #{} {}\n", player_number, resource_index);

    notify_gui(&mut server.clients, &message);
}

pub fn cmd_set(token: Token, server: &mut Server, resource: String)
{
    let (x, y) = {
        let client = server.clients.get(&token).unwrap();
        let player = client.player.as_ref().unwrap();

        (player.x as usize, player.y as usize)
    };

    if !remove_from_inventory(server, token, &resource) {
        send_result(token, server, "ko");
        return;
    }

    drop_resource(server, x, y, &resource);
    send_result(token, server, "ok");
    notify_set_to_gui(token, server, &resource);
}