/*
 *  Project  -  types
 *  Date     -  June 29th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use std::time::SystemTime;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum Resource {
    Food      = 0,
    Linemate  = 1,
    Deraumere = 2,
    Sibur     = 3,
    Mendiane  = 4,
    Phiras    = 5,
    Thystame  = 6,
}

pub const ALL_RESOURCES: [Resource; 7] = [
    Resource::Food,
    Resource::Linemate,
    Resource::Deraumere,
    Resource::Sibur,
    Resource::Mendiane,
    Resource::Phiras,
    Resource::Thystame,
];

impl Resource {
    pub fn from_str(s: &str) -> Option<Self> {
        match s {
            "food"      => Some(Self::Food),
            "linemate"  => Some(Self::Linemate),
            "deraumere" => Some(Self::Deraumere),
            "sibur"     => Some(Self::Sibur),
            "mendiane"  => Some(Self::Mendiane),
            "phiras"    => Some(Self::Phiras),
            "thystame"  => Some(Self::Thystame),
            _           => None,
        }
    }

    pub fn name(self) -> &'static str {
        match self {
            Self::Food      => "food",
            Self::Linemate  => "linemate",
            Self::Deraumere => "deraumere",
            Self::Sibur     => "sibur",
            Self::Mendiane  => "mendiane",
            Self::Phiras    => "phiras",
            Self::Thystame  => "thystame",
        }
    }

    pub fn to_index(self) -> u32 {
        self as u32
    }
}

#[derive(Debug, Clone, Default)]
pub struct Resources(pub [u32; 7]);

impl Resources {
    pub fn get(&self, r: Resource) -> u32 {
        self.0[r as usize]
    }

    pub fn get_mut(&mut self, r: Resource) -> &mut u32 {
        &mut self.0[r as usize]
    }

    pub fn add(&mut self, r: Resource, n: u32) {
        self.0[r as usize] += n;
    }

    pub fn sub_saturating(&mut self, r: Resource, n: u32) {
        self.0[r as usize] = self.0[r as usize].saturating_sub(n);
    }
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
    pub inventory: Resources,
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
    pub resources: Resources,
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

fn shortest_diff(diff: i32, size: i32) -> i32
{
    if diff.abs() <= size / 2 { diff } else if diff > 0 { diff - size } else { diff + size }
}

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
