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

fn command_duration_units(command: &Command) -> u32
{
    match command {
        Command::Forward => 7,
        Command::Right => 7,
        Command::Left => 7,
        Command::Look => 7,
        Command::Inventory => 1,
        Command::Broadcast(_) => 7,
        Command::ConnectNbr => 0,
        Command::Fork => 42,
        Command::Eject => 7,
        Command::Take(_) => 7,
        Command::Set(_) => 7,
        Command::Incantation => 300,
        Command::Unknown => 0,
    }
}

const DEBUG_COMMAND_EXECUTION: bool = true;

fn debug_execute_command(token: Token, is_gui: bool, command: &str)
{
    if !DEBUG_COMMAND_EXECUTION {
        return;
    }

    println!(
        "[EXEC] token={:?} is_gui={} cmd={}",
        token,
        is_gui,
        command
    );
}

fn execute_command(token: Token, server: &mut Server, command_str: String, is_gui: bool)
{
    debug_execute_command(token, is_gui, &command_str);

    if is_gui {
        let cmd = GuiCommand::from_str(&command_str);
        crate::handle_command::handle_gui_command(token, server, cmd);
        return;
    }

    let cmd = Command::from_str(&command_str);
    crate::handle_command::handle_command(token, server, cmd);
}

fn finish_ready_commands(server: &mut Server) -> Vec<(Token, String, bool)>
{
    let now = SystemTime::now();
    let tokens: Vec<Token> = server.clients.keys().copied().collect();
    let mut ready_commands = Vec::new();

    for token in tokens {
        let Some(client) = server.clients.get_mut(&token) else {
            continue;
        };

        if client.is_gui {
            continue;
        }

        let Some(deadline) = client.action_deadline else {
            continue;
        };

        if now < deadline {
            continue;
        }

        client.action_deadline = None;

        if let Some(command) = client.active_command.take() {
            ready_commands.push((token, command, false));
        }
    }

    ready_commands
}

fn start_idle_commands(server: &mut Server) -> Vec<(Token, String, bool)>
{
    let tokens: Vec<Token> = server.clients.keys().copied().collect();
    let frequency = server.params.frequency;
    let mut instant_commands = Vec::new();

    for token in tokens {
        let Some(client) = server.clients.get_mut(&token) else {
            continue;
        };

        if client.is_gui {
            while let Some(command) = client.command_queue.pop_front() {
                instant_commands.push((token, command, true));
            }
            continue;
        }

        if client.player.is_none() {
            continue;
        }

        if client.active_command.is_some() || client.action_deadline.is_some() {
            continue;
        }

        let Some(command_str) = client.command_queue.pop_front() else {
            continue;
        };

        let command = Command::from_str(&command_str);
        let duration_units = command_duration_units(&command);

        if duration_units == 0 {
            instant_commands.push((token, command_str, false));
            continue;
        }

        let duration_ms = calculate_action_duration_ms(duration_units, frequency);

        client.action_deadline = Some(get_deadline(duration_ms));
        client.active_command = Some(command_str);
    }

    instant_commands
}

pub fn process_queued_commands(server: &mut Server)
{
    let ready_commands = finish_ready_commands(server);

    for (token, command, is_gui) in ready_commands {
        execute_command(token, server, command, is_gui);
    }

    let instant_commands = start_idle_commands(server);

    for (token, command, is_gui) in instant_commands {
        execute_command(token, server, command, is_gui);
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
