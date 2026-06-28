/*
 *  Project  -  gui_cmd
 *  Date     -  June 29th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::types::game::{Player, Resource};
use crate::types::network::Server;

pub fn format_bct(server: &Server, x: u32, y: u32) -> String {
    if x < server.params.width && y < server.params.height {
        let tile = &server.world.tiles[y as usize][x as usize];
        let r = &tile.resources;
        format!(
            "bct {} {} {} {} {} {} {} {} {}\n",
            x,
            y,
            r.get(Resource::Food),
            r.get(Resource::Linemate),
            r.get(Resource::Deraumere),
            r.get(Resource::Sibur),
            r.get(Resource::Mendiane),
            r.get(Resource::Phiras),
            r.get(Resource::Thystame),
        )
    } else {
        "sbp\n".to_string()
    }
}

pub fn format_ppo(n: u32, x: u32, y: u32, player: &Player) -> String {
    format!("ppo #{} {} {} {}\n", n, x, y, player.direction.to_num())
}

pub fn format_plv(n: u32, player: &Player) -> String {
    format!("plv #{} {}\n", n, player.level)
}

pub fn format_pin(n: u32, player: &Player) -> String {
    let inv = &player.inventory;
    format!(
        "pin #{} {} {} {} {} {} {} {} {} {}\n",
        n,
        player.x,
        player.y,
        player.food,
        inv.get(Resource::Linemate),
        inv.get(Resource::Deraumere),
        inv.get(Resource::Sibur),
        inv.get(Resource::Mendiane),
        inv.get(Resource::Phiras),
        inv.get(Resource::Thystame),
    )
}
