/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;

use crate::types::game::Resource;
use crate::types::network::{notify_gui, send_result, Server};

fn take_resource(server: &mut Server, x: usize, y: usize, r: Resource) -> bool
{
    let count = server.world.tiles[y][x].resources.get_mut(r);

    if *count == 0 {
        return false;
    }

    *count -= 1;
    true
}

fn add_to_inventory(server: &mut Server, token: Token, r: Resource)
{
    let client = server.clients.get_mut(&token).unwrap();
    let player = client.player.as_mut().unwrap();

    if r == Resource::Food {
        player.food += 1;
        return;
    }

    player.inventory.add(r, 1);
}

fn get_player_position(token: Token, server: &Server) -> (usize, usize)
{
    let client = server.clients.get(&token).unwrap();
    let player = client.player.as_ref().unwrap();

    (player.x as usize, player.y as usize)
}

fn notify_take_to_gui(token: Token, server: &mut Server, r: Resource)
{
    let message = format!("pgt #{} {}\n", token.0, r.to_index());

    notify_gui(&mut server.clients, &message);
}

pub fn cmd_take(token: Token, server: &mut Server, resource: String)
{
    let Some(r) = Resource::from_str(&resource) else {
        send_result(token, server, "ko");
        return;
    };

    let (x, y) = get_player_position(token, server);

    if !take_resource(server, x, y, r) {
        send_result(token, server, "ko");
        return;
    }

    add_to_inventory(server, token, r);
    send_result(token, server, "ok");
    notify_take_to_gui(token, server, r);
}
