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

pub struct Egg
{
    pub team: String,
    pub x: u32,
    pub y: u32,
}

#[derive(Clone, Copy, Debug, PartialEq)]
pub enum Direction
{
    N,
    S,
    E,
    W,
}

impl Direction {
    pub fn to_num(&self) -> u32 {
        match self {
            Direction::N => 1,
            Direction::E => 2,
            Direction::S => 3,
            Direction::W => 4,
        }
    }
}

pub struct Tile {
    pub players: Vec<Token>,
    pub resources: HashMap<String, u32>,
}

pub struct World {
    pub tiles: Vec<Vec<Tile>>,
    pub eggs: Vec<Egg>,
}

pub fn send_response(stream: &mut TcpStream, msg: &str) -> std::io::Result<()> {
    stream.write_all(msg.as_bytes())
}

pub fn notify_gui(clients: &mut HashMap<Token, Client>, msg: &str) {
    for client in clients.values_mut() {
        if client.is_gui {
            let _ = send_response(&mut client.stream, msg);
        }
    }
}

pub fn format_bct(server: &Server, x: u32, y: u32) -> String {
    if x < server.params.width && y < server.params.height {
        let tile = &server.world.tiles[y as usize][x as usize];
        format!(
            "bct {} {} {} {} {} {} {} {} {}\n",
            x,
            y,
            tile.resources.get("food").unwrap_or(&0),
            tile.resources.get("linemate").unwrap_or(&0),
            tile.resources.get("deraumere").unwrap_or(&0),
            tile.resources.get("sibur").unwrap_or(&0),
            tile.resources.get("mendiane").unwrap_or(&0),
            tile.resources.get("phiras").unwrap_or(&0),
            tile.resources.get("thystame").unwrap_or(&0),
        )
    } else {
        "sbp\n".to_string()
    }
}

pub fn format_ppo(n: u32, player: &Player) -> String {
    format!("ppo #{} {} {} {}\n", n, player.x, player.y, player.direction.to_num())
}

pub fn format_plv(n: u32, player: &Player) -> String {
    format!("plv #{} {}\n", n, player.level)
}

pub fn format_pin(n: u32, player: &Player) -> String {
    format!(
        "pin #{} {} {} {} {} {} {} {} {} {}\n",
        n,
        player.x,
        player.y,
        player.food,
        player.inventory.get("linemate").unwrap_or(&0),
        player.inventory.get("deraumere").unwrap_or(&0),
        player.inventory.get("sibur").unwrap_or(&0),
        player.inventory.get("mendiane").unwrap_or(&0),
        player.inventory.get("phiras").unwrap_or(&0),
        player.inventory.get("thystame").unwrap_or(&0),
    )
}

