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

    {
        let client = server.clients.get_mut(&token).unwrap();
        let mut buf = [0u8; 1024];

        match client.stream.read(&mut buf) {
            Ok(0) => {
                server.clients.remove(&token);
                return Ok(());
            }
            Ok(n) => {
                let data = String::from_utf8_lossy(&buf[..n]);
                client.buffer.push_str(&data);
            }
            Err(e) => return Err(e),
        }

        while let Some(pos) = client.buffer.find('\n') {
            let req = client.buffer[..pos].trim_end_matches('\r').to_string();
            client.buffer = client.buffer[pos + 1..].to_string();
            requests.push(req);
        }
    }

    for req in requests {
        let client = server.clients.get(&token).unwrap();
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

fn handle_handshake(token: Token, server: &mut Server, team: String)
{
    let valid_team = server.params.teams_names.contains(&team) || team == "GRAPHIC";
    let available_clients_slots = server.params.team_clients_nb - server.clients.len() as u32;
    let rand_x = token.0 as u32 % server.params.width;
    let rand_y = token.0 as u32 % server.params.height;

    if server.params.teams_names.contains(&team) {
        valid_team = true;
    } else if team == "GRAPHIC" {
        valid_team = true;
    }

    let client = server.clients.get_mut(&token).unwrap();

    if valid_team {
        client.team_name = Some(team.clone());
        if team == "GRAPHIC" {
            client.is_gui = true;
        } else {
            client.player = Some(utils::Player {
                x: rand_x,
                y: rand_y,
                direction: utils::Direction::N,
                level: 1,
                food: 10,
                inventory: std::collections::HashMap::new(),
            });
            let res = format!("{}\n{} {}\n", available_clients_slots, server.params.width, server.params.height);
            let _ = utils::send_response(&mut client.stream, &res);
        }
    } else {
        let _ = utils::send_response(&mut client.stream, "ko\n");
    }
}
