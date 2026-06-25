/*
** Project  -  Zappy
** Date     -  Jun 1, 2026
**
** Copyright (c) 2026 Léo Lacordaire
*/

use mio::Token;
use mio::net::TcpStream;
use std::collections::HashMap;
use std::io::Write;

#[derive(Clone)]

pub struct ServerParams
{
    pub port: u16,
    pub width: u32,
    pub height: u32,
    pub teams_names: Vec<String>,
    pub team_clients_nb: u32,
    pub frequency: u32,
}

pub struct Client {
    pub stream: TcpStream,
    pub buffer: String,
    pub team_name: Option<String>,
    pub player: Option<Player>,
    pub is_gui: bool,
}

pub struct Server {
    pub clients: HashMap<Token, Client>,
    pub params: ServerParams,
    pub world: World,
}

pub struct Player
{
    pub x: u32,
    pub y: u32,
    pub direction: Direction,
    pub level: u32,
    pub food: u32,
    pub inventory: HashMap<String, u32>,
}

pub enum Direction
{
    N,
    S,
    E,
    W,
}

pub struct Tile {
    pub players: Vec<Token>,
    pub resources: HashMap<String, u32>,
}

pub struct World {
    pub tiles: Vec<Vec<Tile>>,
}

pub fn send_response(stream: &mut TcpStream, msg: &str) -> std::io::Result<()> {
    stream.write_all(msg.as_bytes())
}
