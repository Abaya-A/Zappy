/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Player, Server, send_result};
use crate::timers;
use std::collections::HashMap;

struct IncantationRequirements {
    players: usize,
    resources: HashMap<String, u32>,
}

fn get_requirements(level: u32) -> Option<IncantationRequirements> {
    let mut resources = HashMap::new();
    let (players, reqs) = match level {
        1 => (1, vec![("linemate", 1)]),
        2 => (2, vec![("linemate", 1), ("deraumere", 1), ("sibur", 1)]),
        3 => (2, vec![("linemate", 2), ("sibur", 1), ("phiras", 2)]),
        4 => (4, vec![("linemate", 1), ("deraumere", 1), ("sibur", 2), ("phiras", 1)]),
        5 => (4, vec![("linemate", 1), ("deraumere", 2), ("sibur", 1), ("mendiane", 3)]),
        6 => (6, vec![("linemate", 1), ("deraumere", 2), ("sibur", 3), ("phiras", 1)]),
        7 => (6, vec![("linemate", 2), ("deraumere", 2), ("sibur", 2), ("mendiane", 2), ("phiras", 2), ("thystame", 1)]),
        _ => return None,
    };

    for (name, count) in reqs {
        resources.insert(name.to_string(), count);
    }

    Some(IncantationRequirements{players, resources})
}

fn check_incantation(server: &Server, player: &Player) -> bool {
    let requirements = match get_requirements(player.level) {
        Some(req) => req,
        None => return false,
    };

    let tile = &server.world.tiles[player.y as usize][player.x as usize];

    for (resource, required_count) in &requirements.resources {
        if tile.resources.get(resource).cloned().unwrap_or(0) < *required_count {
            return false;
        }
    }

    let players_on_tile: Vec<_> = tile
        .players
        .iter()
        .filter_map(|token| {
            server
                .clients
                .get(token)
                .and_then(|c| c.player.as_ref())
        })
        .collect();

    let mut same_level_players = 0;
    for p in players_on_tile {
        if p.level == player.level {
            same_level_players += 1;
        }
    }

    same_level_players >= requirements.players
}

pub fn cmd_incantation(token: Token, server: &mut Server) {
    if !timers::can_act(token, server) {
        send_result(token, server, "ko");
        return;
    }

    let player = match server.clients.get(&token).and_then(|c| c.player.clone()) {
        Some(p) => p,
        None => {
            send_result(token, server, "ko");
            return;
        }
    };

    if check_incantation(server, &player) {
        let response = format!("Elevation underway\nCurrent level: {}", player.level + 1);
        send_result(token, server, &response);
        timers::start_action(token, server, 300);
    } else {
        send_result(token, server, "ko");
    }
}
