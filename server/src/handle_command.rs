/*
 *  Project  -  src
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use crate::utils::Server;
use mio::Token;

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
    Unknown,
}

#[derive(Debug)]
pub enum GuiCommand {
    Msz,
    Bct(u32, u32),
    Mct,
    Tna,
    Ppo(u32),
    Plv(u32),
    Pin(u32),
    Sgt,
    Sst(u32),
    Unknown,
}

impl Command {
    pub fn from_str(s: &str) -> Self {
        let parts: Vec<&str> = s.split_whitespace().collect();
        if parts.is_empty() {
            return Command::Unknown;
        }

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

impl GuiCommand {
    pub fn from_str(s: &str) -> Self {
        let parts: Vec<&str> = s.split_whitespace().collect();
        if parts.is_empty() {
            return GuiCommand::Unknown;
        }

        match parts[0] {
            "msz" => GuiCommand::Msz,
            "bct" if parts.len() == 3 => {
                if let (Ok(x), Ok(y)) = (parts[1].parse::<u32>(), parts[2].parse::<u32>()) {
                    GuiCommand::Bct(x, y)
                } else {
                    GuiCommand::Unknown
                }
            }
            "mct" => GuiCommand::Mct,
            "tna" => GuiCommand::Tna,
            "ppo" if parts.len() == 2 => {
                if let Ok(n) = parts[1].parse::<u32>() {
                    GuiCommand::Ppo(n)
                } else {
                    GuiCommand::Unknown
                }
            }
            "plv" if parts.len() == 2 => {
                if let Ok(n) = parts[1].parse::<u32>() {
                    GuiCommand::Plv(n)
                } else {
                    GuiCommand::Unknown
                }
            }
            "pin" if parts.len() == 2 => {
                if let Ok(n) = parts[1].parse::<u32>() {
                    GuiCommand::Pin(n)
                } else {
                    GuiCommand::Unknown
                }
            }
            "sgt" => GuiCommand::Sgt,
            "sst" if parts.len() == 2 => {
                if let Ok(t) = parts[1].parse::<u32>() {
                    GuiCommand::Sst(t)
                } else {
                    GuiCommand::Unknown
                }
            }
            _ => GuiCommand::Unknown,
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

pub fn handle_gui_command(token: Token, server: &mut Server, cmd: GuiCommand) {
    match cmd {
        GuiCommand::Msz => crate::gui_cmd::msz::cmd_msz(token, server),
        GuiCommand::Bct(x, y) => crate::gui_cmd::bct::cmd_bct(token, server, x, y),
        GuiCommand::Mct => crate::gui_cmd::mct::cmd_mct(token, server),
        GuiCommand::Tna => crate::gui_cmd::tna::cmd_tna(token, server),
        GuiCommand::Ppo(n) => crate::gui_cmd::ppo::cmd_ppo(token, server, n),
        GuiCommand::Plv(n) => crate::gui_cmd::plv::cmd_plv(token, server, n),
        GuiCommand::Pin(n) => crate::gui_cmd::pin::cmd_pin(token, server, n),
        GuiCommand::Sgt => crate::gui_cmd::sgt::cmd_sgt(token, server),
        GuiCommand::Sst(t) => crate::gui_cmd::sst::cmd_sst(token, server, t),
        GuiCommand::Unknown => crate::gui_cmd::unknown_gui::cmd_unknown_gui(token, server),
    }
}
