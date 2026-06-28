/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;

use crate::types::game::Resource;
use crate::types::network::{notify_gui, send_result, Server};
use crate::gui_cmd::format_cmd::format_bct;

fn remove_from_inventory(server: &mut Server, token: Token, r: Resource) -> bool
{
    let client = server.clients.get_mut(&token).unwrap();
    let player = client.player.as_mut().unwrap();

    if r == Resource::Food {
        if player.food == 0 {
            return false;
        }
        player.food -= 1;
        return true;
    }

    let count = player.inventory.get_mut(r);

    if *count == 0 {
        return false;
    }

    *count -= 1;
    true
}

fn drop_resource(server: &mut Server, x: usize, y: usize, r: Resource)
{
    server.world.tiles[y][x].resources.add(r, 1);
}

fn notify_set_to_gui(token: Token, server: &mut Server, r: Resource)
{
    let message = format!("pdr #{} {}\n", token.0, r.to_index());

    notify_gui(&mut server.clients, &message);
}

pub fn cmd_set(token: Token, server: &mut Server, resource: String)
{
    let Some(r) = Resource::from_str(&resource) else {
        send_result(token, server, "ko");
        return;
    };

    let (x, y) = {
        let client = server.clients.get(&token).unwrap();
        let player = client.player.as_ref().unwrap();

        (player.x as usize, player.y as usize)
    };

    if !remove_from_inventory(server, token, r) {
        send_result(token, server, "ko");
        return;
    }

    drop_resource(server, x, y, r);
    send_result(token, server, "ok");
    notify_set_to_gui(token, server, r);
    let bct = format_bct(server, x as u32, y as u32);
    notify_gui(&mut server.clients, &bct);
}
