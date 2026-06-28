/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;

use crate::utils::{
    notify_gui,
    resource_to_index,
    send_result,
    Server,
};

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

fn get_player_position(token: Token, server: &Server) -> (usize, usize)
{
    let client = server.clients.get(&token).unwrap();
    let player = client.player.as_ref().unwrap();

    (player.x as usize, player.y as usize)
}

fn notify_take_to_gui(token: Token, server: &mut Server, resource: &str)
{
    let player_number = token.0 as u32;
    let resource_index = resource_to_index(resource);
    let message = format!("pgt #{} {}\n", player_number, resource_index);

    notify_gui(&mut server.clients, &message);
}

pub fn cmd_take(token: Token, server: &mut Server, resource: String)
{
    let (x, y) = get_player_position(token, server);

    if !take_resource(server, x, y, &resource) {
        send_result(token, server, "ko");
        return;
    }

    add_to_inventory(server, token, &resource);
    send_result(token, server, "ok");
    notify_take_to_gui(token, server, &resource);
}