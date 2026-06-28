/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin & Léo Lacordaire
 */

use mio::Token;

use crate::gui_cmd::format_cmd::format_pin;
use crate::types::game::Resource;
use crate::types::network::{notify_gui, send_result, Server};

fn build_inventory_response(token: Token, server: &Server) -> (String, String)
{
    let client = server.clients.get(&token).unwrap();
    let player = client.player.as_ref().unwrap();
    let player_number = token.0 as u32;

    let inv = &player.inventory;
    let response = format!(
        "[food {}, linemate {}, deraumere {}, sibur {}, mendiane {}, phiras {}, thystame {}]",
        player.food,
        inv.get(Resource::Linemate),
        inv.get(Resource::Deraumere),
        inv.get(Resource::Sibur),
        inv.get(Resource::Mendiane),
        inv.get(Resource::Phiras),
        inv.get(Resource::Thystame),
    );

    let pin = format_pin(player_number, player);

    (response, pin)
}

pub fn cmd_inventory(token: Token, server: &mut Server)
{
    let (response, pin) = build_inventory_response(token, server);

    send_result(token, server, &response);
    notify_gui(&mut server.clients, &pin);
}
