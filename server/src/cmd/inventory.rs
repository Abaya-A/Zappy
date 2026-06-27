/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin & Léo Lacordaire
 */

use mio::Token;
use crate::utils::{Server, send_result};
use crate::timers;

pub fn cmd_inventory(token: Token, server: &mut Server)
{
    if !timers::can_act(token, server) {
        send_result(token, server, "ko");
        return;
    }

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

    send_result(token, server, &response.trim_end_matches('\n'));
    timers::start_action(token, server, 1);
}
