/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin & Léo Lacordaire
 */

use mio::Token;
use crate::utils::{Server, send_response};

pub fn cmd_inventory(token: Token, server: &mut Server)
{
    let client = server.clients.get_mut(&token).unwrap();
    let player = client.player.as_ref().unwrap();

    let response = format!(
        "[food {}, linemate {}, deraumere {}, sibur {}, mendiane {}, phiras {}, thystame {}]\n",
        player.food,
        player.inventory.get("linemate").unwrap_or(&0),
        player.inventory.get("deraumere").unwrap_or(&0),
        player.inventory.get("sibur").unwrap_or(&0),
        player.inventory.get("mendiane").unwrap_or(&0),
        player.inventory.get("phiras").unwrap_or(&0),
        player.inventory.get("thystame").unwrap_or(&0),
    );

    let _ = send_response(&mut client.stream, &response);
}
