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
}

#[derive(Debug, Clone)]
pub struct Team {
    pub name: String,
    pub available_slots: usize,
}

#[derive(Debug, Clone)]
pub struct Player
{
    pub x: u32,
    pub y: u32,
    pub direction: Direction,
    pub level: u32,
    pub food: u32,
    pub inventory: HashMap<String, u32>,
    pub last_food_update: SystemTime,
}

#[derive(Debug)]
pub struct World {
    pub tiles: Vec<Vec<Tile>>,
    pub eggs: Vec<Egg>,
}

#[derive(Debug)]
pub struct Egg
{
    pub team: String,
    pub x: u32,
    pub y: u32,
}

#[derive(Debug)]
pub struct Tile {
    pub players: Vec<Token>,
    pub resources: HashMap<String, u32>,
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

pub fn send_response(stream: &mut TcpStream, response: &str) -> std::io::Result<()>
{
    stream.write_all(response.as_bytes())
}

pub fn notify_gui(clients: &mut HashMap<Token, Client>, msg: &str)
{
    for client in clients.values_mut() {
        if client.is_gui {
            let _ = send_response(&mut client.stream, msg);
        }
    }
    println!("GUI notified: {}", msg);
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

pub fn format_ppo(n: u32, x: u32, y: u32, player: &Player) -> String {
    format!("ppo #{} {} {} {}\n", n, x, y, player.direction.to_num())
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

pub fn send_result(token: Token, server: &mut Server, state: &str)
{
    let client = server.clients.get_mut(&token).unwrap();
    let _ = send_response(&mut client.stream, &format!("{state}\n"));
}


pub fn resource_to_index(name: &str) -> u32
{
    match name {
        "food"      => 0,
        "linemate"  => 1,
        "deraumere" => 2,
        "sibur"     => 3,
        "mendiane"  => 4,
        "phiras"    => 5,
        "thystame"  => 6,
        _           => 0,
    }
}

fn shortest_diff(diff: i32, size: i32) -> i32
{
    if diff.abs() <= size / 2 { diff } else if diff > 0 { diff - size } else { diff + size }
}


// compute direction

fn angle_to_tile(angle: f64) -> u32
{
    match angle as u32 {
        0..=22 | 338..=360 => 1,
        23..=67            => 2,
        68..=112           => 3,
        113..=157          => 4,
        158..=202          => 5,
        203..=247          => 6,
        248..=292          => 7,
        _                  => 8,
    }
}

pub fn compute_direction(ex: u32, ey: u32, rx: u32, ry: u32,rdir: &Direction,width: u32, height: u32) -> u32
{
    if ex == rx && ey == ry {
        return 0;
    }
    
    let dx = shortest_diff(ex as i32 - rx as i32, width as i32);
    let dy = shortest_diff(ey as i32 - ry as i32, height as i32);

    let angle = ((-dy as f64).atan2(dx as f64).to_degrees() + 360.0) % 360.0;

    let offset = match rdir {
        Direction::N => 0.0,
        Direction::E => 90.0,
        Direction::S => 180.0,
        Direction::W => 270.0,
    };

    angle_to_tile((angle - offset + 360.0) % 360.0)
}