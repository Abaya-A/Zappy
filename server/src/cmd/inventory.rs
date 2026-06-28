/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin & Léo Lacordaire
 */

use mio::Token;

use crate::utils::{
    format_pin,
    notify_gui,
    send_result,
    Server,
};

fn build_inventory_response(token: Token, server: &Server) -> (String, String)
{
    let client = server.clients.get(&token).unwrap();
    let player = client.player.as_ref().unwrap();
    let player_number = token.0 as u32;

    let response = format!(
        "[food {}, linemate {}, deraumere {}, sibur {}, mendiane {}, phiras {}, thystame {}]",
        player.food,
        player.inventory.get("linemate").unwrap_or(&0),
        player.inventory.get("deraumere").unwrap_or(&0),
        player.inventory.get("sibur").unwrap_or(&0),
        player.inventory.get("mendiane").unwrap_or(&0),
        player.inventory.get("phiras").unwrap_or(&0),
        player.inventory.get("thystame").unwrap_or(&0),
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