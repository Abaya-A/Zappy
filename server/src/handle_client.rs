/*
 *  Project  -  src
 *  Date     -  June 9th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::handle_command;
use crate::handle_command::{Command, GuiCommand, handle_gui_command};
use crate::utils;
use crate::utils::Server;
use mio::Token;
use std::io::Read;

pub fn handle_client(token: Token, server: &mut Server) -> Result<(), std::io::Error> {
    let mut requests = Vec::new();
    let mut closed = false;

    {
        let client = match server.clients.get_mut(&token) {
            Some(c) => c,
            None => return Ok(()),
        };
        let mut buf = [0u8; 1024];

        match client.stream.read(&mut buf) {
            Ok(0) => {
                closed = true;
            }
            Ok(n) => {
                let data = String::from_utf8_lossy(&buf[..n]);
                client.buffer.push_str(&data);
            }
            Err(_) => {
                closed = true;
            }
        }

        if !closed {
            while let Some(pos) = client.buffer.find('\n') {
                let req = client.buffer[..pos].trim_end_matches('\r').to_string();
                client.buffer = client.buffer[pos + 1..].to_string();
                requests.push(req);
            }
        }
    }

    if closed {
        cleanup_client(token, server);
        return Ok(());
    }

    for req in requests {
        let client = match server.clients.get(&token) {
            Some(c) => c,
            None => continue,
        };
        let is_auth = client.team_name.is_some();
        let is_gui = client.is_gui;

        if is_auth {
            if is_gui {
                let cmd = GuiCommand::from_str(&req);
                handle_gui_command(token, server, cmd);
            } else {
                let cmd = Command::from_str(&req);
                handle_command::handle_command(token, server, cmd);
            }
        } else {
            handle_handshake(token, server, req);
        }
    }

    Ok(())
}

fn handle_handshake(token: Token, server: &mut Server, team: String) {
    let valid_team = server.params.teams_names.contains(&team) || team == "GRAPHIC";

    if valid_team {
        if team == "GRAPHIC" {
            let mut existing_players = String::new();
            for (t, c) in &server.clients {
                if *t != token {
                    if let (Some(player), Some(t_name)) = (&c.player, &c.team_name) {
                        existing_players.push_str(&format!(
                            "pnw #{} {} {} {} {} {}\n",
                            t.0,
                            player.x,
                            player.y,
                            player.direction.to_num(),
                            player.level,
                            t_name
                        ));
                    }
                }
            }
            let client = server.clients.get_mut(&token).unwrap();
            client.team_name = Some(team);
            client.is_gui = true;
            if !existing_players.is_empty() {
                let _ = utils::send_response(&mut client.stream, &existing_players);
            }
        } else {
            if let Some(team_data) = server.teams.iter_mut().find(|t| t.name == team) {
                if team_data.available_slots == 0 {
                    let client = server.clients.get_mut(&token).unwrap();
                    let _ = utils::send_response(&mut client.stream, "ko\n");
                    return;
                }
                team_data.available_slots -= 1;
            }

            let egg_pos = server.world.eggs.iter().position(|e| e.team == team);
            let (rand_x, rand_y) = match egg_pos {
                Some(i) => {
                    let egg = server.world.eggs.remove(i);
                    (egg.x, egg.y)
                }
                None => {
                    let seed = std::time::SystemTime::now()
                        .duration_since(std::time::SystemTime::UNIX_EPOCH)
                        .unwrap().subsec_nanos();
                    (seed % server.params.width, (seed / server.params.width) % server.params.height)
                }
            };

            let total_available_slots = server.teams.iter().find(|t| t.name == team).map(|t| t.available_slots).unwrap_or(0);

            let client = server.clients.get_mut(&token).unwrap();

            client.team_name = Some(team.clone());
            client.player = Some(utils::Player {
                x: rand_x,
                y: rand_y,
                direction: utils::Direction::N,
                level: 1,
                food: 10,
                inventory: std::collections::HashMap::new(),
                last_food_update: std::time::SystemTime::now(),
            });

            server.world.tiles[rand_y as usize][rand_x as usize].players.push(token);

            let res = format!("{}\n{} {}\n", total_available_slots, server.params.width, server.params.height);
            let _ = utils::send_response(&mut client.stream, &res);

            let pnw_msg = format!("pnw #{} {} {} {} 1 {}\n", token.0, rand_x, rand_y, utils::Direction::N.to_num(), team);
            utils::notify_gui(&mut server.clients, &pnw_msg);
        }
    } else {
        let client = server.clients.get_mut(&token).unwrap();
        let _ = utils::send_response(&mut client.stream, "ko\n");
    }
}

fn cleanup_client(token: Token, server: &mut Server) {
    if let Some(removed_client) = server.clients.remove(&token) {
        if let Some(player) = &removed_client.player {
            let px = player.x as usize;
            let py = player.y as usize;
            if py < server.world.tiles.len() && px < server.world.tiles[py].len() {
                server.world.tiles[py][px].players.retain(|&t| t != token);
            }
            let death_msg = format!("pdi #{}\n", token.0);
            utils::notify_gui(&mut server.clients, &death_msg);
        }
    }
}

