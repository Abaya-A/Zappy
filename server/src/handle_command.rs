/*
 *  Project  -  src
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::Server;

#[derive(Debug)]
pub enum Command {
    Forward,
    Right,
    Left,
    Look,
    Inventory,
    Broadcast(String),
    ConnectNbr,
    Fork,
    Eject,
    Take(String),
    Set(String),
    Incantation,
    Unknown
}

impl Command {
    pub fn from_str(s: &str) -> Self {
        let parts: Vec<&str> = s.split_whitespace().collect();
        if parts.is_empty() { return Command::Unknown; }

        match parts[0] {
            "Forward" => Command::Forward,
            "Right" => Command::Right,
            "Left" => Command::Left,
            "Look" => Command::Look,
            "Inventory" => Command::Inventory,
            "Broadcast" if parts.len() > 1 => Command::Broadcast(parts[1..].join(" ")),
            "Connect_nbr" => Command::ConnectNbr,
            "Fork" => Command::Fork,
            "Eject" => Command::Eject,
            "Take" if parts.len() > 1 => Command::Take(parts[1..].join(" ")),
            "Set" if parts.len() > 1 => Command::Set(parts[1..].join(" ")),
            "Incantation" => Command::Incantation,
            _ => Command::Unknown,
        }
    }
}

pub fn handle_command(token: Token, server: &mut Server, cmd: Command) {
    match cmd {
        Command::Forward => crate::cmd::forward::cmd_forward(token, server),
        Command::Right => crate::cmd::right::cmd_right(token, server),
        Command::Left => crate::cmd::left::cmd_left(token, server),
        Command::Look => crate::cmd::look::cmd_look(token, server),
        Command::Inventory => crate::cmd::inventory::cmd_inventory(token, server),
        Command::ConnectNbr => crate::cmd::connect_nbr::cmd_connect_nbr(token, server),
        Command::Broadcast(msg) => crate::cmd::broadcast::cmd_broadcast(token, server, msg),
        Command::Fork => crate::cmd::fork::cmd_fork(token, server),
        Command::Eject => crate::cmd::eject::cmd_eject(token, server),
        Command::Take(obj) => crate::cmd::take::cmd_take(token, server, obj),
        Command::Set(obj) => crate::cmd::set::cmd_set(token, server, obj),
        Command::Incantation => crate::cmd::incantation::cmd_incantation(token, server),
        Command::Unknown => crate::cmd::unknown::cmd_unknown(token, server),
    }
}

