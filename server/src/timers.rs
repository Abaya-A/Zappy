/*
 *  Project  -  src
 *  Date     -  June 28th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::utils::Server;
use crate::handle_command::{Command, GuiCommand};
use std::time::{SystemTime, Duration};
use mio::Token;

pub fn calculate_action_duration_ms(action_units: u32, frequency: u32) -> u64 {
    ((action_units as f64 / frequency as f64) * 1000.0) as u64
}

pub fn get_deadline(action_ms: u64) -> SystemTime {
    SystemTime::now() + Duration::from_millis(action_ms)
}

pub fn ms_until_deadline(deadline: SystemTime) -> Option<u64> {
    deadline
        .duration_since(SystemTime::now())
        .ok()
        .map(|d| d.as_millis() as u64)
}

pub fn can_act(token: Token, server: &Server) -> bool {
    if let Some(client) = server.clients.get(&token) {
        match client.action_deadline {
            Some(deadline) => SystemTime::now() >= deadline,
            None => true,
        }
    } else {
        false
    }
}

pub fn process_queued_commands(server: &mut Server) {
    let now = SystemTime::now();
    let mut commands_to_process: Vec<(Token, String, bool)> = Vec::new();

    // Push to the queue the commands
    for (token, client) in server.clients.iter_mut() {
        if let Some(deadline) = client.action_deadline {
            if now >= deadline {
                client.action_deadline = None;
                if let Some(command_str) = client.command_queue.pop_front() {
                    let is_gui = client.is_gui;
                    commands_to_process.push((*token, command_str, is_gui));
                }
            }
        } else if !client.command_queue.is_empty() {
            if let Some(command_str) = client.command_queue.pop_front() {
                let is_gui = client.is_gui;
                commands_to_process.push((*token, command_str, is_gui));
            }
        }
    }

    // Process the commands
    for (token, command_str, is_gui) in commands_to_process {
        if is_gui {
            let cmd = GuiCommand::from_str(&command_str);
            crate::handle_command::handle_gui_command(token, server, cmd);
        } else {
            let cmd = Command::from_str(&command_str);
            crate::handle_command::handle_command(token, server, cmd);
        }
    }
}

pub fn start_action(token: Token, server: &mut Server, action_units: u32) {
    if let Some(client) = server.clients.get_mut(&token) {
        let duration_ms = calculate_action_duration_ms(action_units, server.params.frequency);
        client.action_deadline = Some(get_deadline(duration_ms));
    }
}

pub fn verify_player_hunger(server: &mut Server)
{
    let now = SystemTime::now();
    let mut dead_players = Vec::new();
    for (token, client) in server.clients.iter_mut() {
        if now >= client.hunger_check_deadline {
            if let Some(player) = &mut client.player {
                if let Ok(elapsed) = player.last_food_update.elapsed() {
                    let elapsed_s = elapsed.as_secs_f64();
                    let game_time = elapsed_s * server.params.frequency as f64;
                    let food_consumed = (game_time / 126.0) as u32;
                    if food_consumed > 0 {  // ← ajout ici
                        if player.food >= food_consumed {
                            player.food -= food_consumed;
                        } else {
                            player.food = 0;
                            dead_players.push(*token);
                        }
                        player.last_food_update = SystemTime::now();
                    }
                }
            }
            client.hunger_check_deadline = get_deadline(100);
        }
        if let Some(deadline) = client.action_deadline {
            if now >= deadline {
                client.action_deadline = None;
            }
        }
    }
    for dead_token in dead_players {
        if let Some(mut client) = server.clients.remove(&dead_token) {
            let _ = crate::utils::send_response(&mut client.stream, "dead\n");
            let _ = client.stream.shutdown(std::net::Shutdown::Both);
        }
    }
}

pub fn get_poll_timeout(server: &mut Server) -> Option<Duration>
{
    let mut min_timeout_ms = u64::MAX;

    for client in server.clients.values() {
        if let Some(deadline) = client.action_deadline {
            if let Some(ms) = ms_until_deadline(deadline) {
                if ms > 0 {
                    min_timeout_ms = min_timeout_ms.min(ms);
                }
            }
        }

        if let Some(ms) = ms_until_deadline(client.hunger_check_deadline) {
            if ms > 0 {
                min_timeout_ms = min_timeout_ms.min(ms);
            }
        }
    }

    let timeout = if min_timeout_ms == u64::MAX {
        None
    } else {
        Some(Duration::from_millis(min_timeout_ms))
    };
    timeout
}
