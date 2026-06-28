/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin & Léo Lacordaire
 */

use mio::Token;
use crate::utils::{Server, send_result, format_pin, notify_gui};
use crate::timers;

pub fn cmd_inventory(token: Token, server: &mut Server)
{
    if !timers::can_act(token, server) {
        send_result(token, server, "ko");
        return;
    }

    let (response, pin) = {
        let client = server.clients.get(&token).unwrap();
        let player = client.player.as_ref().unwrap();
        let n = token.0 as u32;
        let r = format!(
            "[food {}, linemate {}, deraumere {}, sibur {}, mendiane {}, phiras {}, thystame {}]\n",
            player.food,
            player.inventory.get("linemate").unwrap_or(&0),
            player.inventory.get("deraumere").unwrap_or(&0),
            player.inventory.get("sibur").unwrap_or(&0),
            player.inventory.get("mendiane").unwrap_or(&0),
            player.inventory.get("phiras").unwrap_or(&0),
            player.inventory.get("thystame").unwrap_or(&0),
        );
        let pin = format_pin(n, player);
        (r, pin)
    };

    send_result(token, server, response.trim_end_matches('\n'));
    timers::start_action(token, server, 1);
    notify_gui(&mut server.clients, &pin);
}
