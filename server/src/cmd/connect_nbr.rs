/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Server, send_response};

pub fn cmd_connect_nbr(token: Token, server: &mut Server)
{
    let client = match server.clients.get(&token) {
        Some(c) => c,
        None => return,
    };

    let team_name = match &client.team_name {
        Some(name) => name.clone(),
        None => return,
    };

    let available_slots = server.teams.iter().find(|t| t.name == team_name).map(|t| t.available_slots).unwrap_or(0);

    if let Some(client) = server.clients.get_mut(&token) {
        let response = format!("{}\n", available_slots);
        send_response(&mut client.stream, &response).unwrap_or_default();
    }
}
