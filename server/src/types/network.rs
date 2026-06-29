/*
 *  Project  -  types
 *  Date     -  June 29th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use super::game::{World, Team, Player};
use mio::Token;
use mio::net::TcpStream;
use std::collections::{HashMap, VecDeque};
use std::io::Write;
use std::time::SystemTime;

#[derive(Clone, Debug)]
pub struct ServerParams
{
    pub port: u16,
    pub width: u32,
    pub height: u32,
    pub teams_names: Vec<String>,
    pub team_clients_nb: u32,
    pub frequency: u32,
}

#[derive(Debug)]
pub struct Server {
    pub clients: HashMap<Token, Client>,
    pub params: ServerParams,
    pub world: World,
    pub teams: Vec<Team>,
}

#[derive(Debug)]
pub struct Client {
    pub stream: TcpStream,
    pub buffer: String,
    pub team_name: Option<String>,
    pub player: Option<Player>,
    pub is_gui: bool,
    pub action_deadline: Option<SystemTime>,
    pub hunger_check_deadline: SystemTime,
    pub command_queue: VecDeque<String>,
    pub active_command: Option<String>,
}

pub fn send_response(stream: &mut TcpStream, response: &str) -> std::io::Result<()>
{
    println!("[IA NOTIF] {}", response.trim_end());
    stream.write_all(response.as_bytes())
}

pub fn send_result(token: Token, server: &mut Server, state: &str)
{
    let client = server.clients.get_mut(&token).unwrap();
    let _ = send_response(&mut client.stream, &format!("{state}\n"));
}

pub fn notify_gui(clients: &mut HashMap<Token, Client>, msg: &str)
{
    for client in clients.values_mut() {
        if client.is_gui {
            let _ = send_response(&mut client.stream, msg);
        }
    }
    println!("[GUI NOTIF] {}", msg);
}
