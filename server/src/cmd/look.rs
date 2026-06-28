/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;

use crate::utils::{send_result, Direction, Server};

type Position = (u32, u32);
type Vector = (i32, i32);

const DEBUG_LOOK: bool = true;

fn debug_look_coordinates(token: Token, vision: &[Vec<Position>])
{
    if !DEBUG_LOOK {
        return;
    }

    let mut index = 0;

    print!("[LOOK COORDS] token={:?}", token);

    for line in vision {
        for &(x, y) in line {
            print!(" {}=({}, {})", index, x, y);
            index += 1;
        }
    }

    println!();
}

fn debug_look_response(token: Token, response: &str)
{
    if !DEBUG_LOOK {
        return;
    }

    println!("[LOOK RESPONSE] token={:?} {}", token, response);
}

fn get_player(token: Token, server: &Server) -> (u32, u32, Direction, u32)
{
    let client = server.clients.get(&token).unwrap();
    let player = client.player.as_ref().unwrap();

    (player.x, player.y, player.direction.clone(), player.level)
}

fn wrap_coordinate(value: i32, max: u32) -> u32
{
    value.rem_euclid(max as i32) as u32
}

fn direction_vectors(direction: Direction) -> (Vector, Vector)
{
    match direction {
        Direction::N => ((0, -1), (-1, 0)),
        Direction::E => ((1, 0), (0, -1)),
        Direction::S => ((0, 1), (1, 0)),
        Direction::W => ((-1, 0), (0, 1)),
    }
}

fn get_tile_position(
    x: u32,
    y: u32,
    forward: Vector,
    left: Vector,
    distance: u32,
    offset: i32,
    width: u32,
    height: u32,
) -> Position {
    let distance = distance as i32;

    let target_x = x as i32
        + forward.0 * distance
        + left.0 * offset;

    let target_y = y as i32
        + forward.1 * distance
        + left.1 * offset;

    (
        wrap_coordinate(target_x, width),
        wrap_coordinate(target_y, height),
    )
}

fn get_line_of_view(
    server: &Server,
    x: u32,
    y: u32,
    direction: Direction,
    distance: u32,
) -> Vec<Position> {
    let width = server.params.width;
    let height = server.params.height;
    let (forward, left) = direction_vectors(direction);
    let mut tiles = Vec::new();

    for offset in (-(distance as i32)..=(distance as i32)).rev() {
            tiles.push(get_tile_position(
            x,
            y,
            forward,
            left,
            distance,
            offset,
            width,
            height,
        ));
    }

    tiles
}

fn build_look(token: Token, server: &Server) -> Vec<Vec<Position>>
{
    let (x, y, direction, level) = get_player(token, server);
    let mut vision = Vec::new();

    for distance in 0..=level {
        vision.push(get_line_of_view(
            server,
            x,
            y,
            direction.clone(),
            distance,
        ));
    }

    vision
}

fn append_players(items: &mut String, player_count: usize)
{
    for _ in 0..player_count {
        if !items.is_empty() {
            items.push(' ');
        }

        items.push_str("player");
    }
}

fn append_resources(items: &mut String, server: &Server, x: u32, y: u32)
{
    let tile = &server.world.tiles[y as usize][x as usize];

    for (resource, count) in &tile.resources {
        for _ in 0..*count {
            if !items.is_empty() {
                items.push(' ');
            }

            items.push_str(resource);
        }
    }
}

fn format_tile(server: &Server, x: u32, y: u32) -> String
{
    let tile = &server.world.tiles[y as usize][x as usize];
    let mut items = String::new();

    append_players(&mut items, tile.players.len());
    append_resources(&mut items, server, x, y);

    items
}

fn format_look_response(server: &Server, vision: &[Vec<Position>]) -> String
{
    let mut result = String::from("[");
    let mut first = true;

    for line in vision {
        for &(x, y) in line {
            if !first {
                result.push_str(", ");
            }

            first = false;
            result.push_str(&format_tile(server, x, y));
        }
    }

    result.push(']');
    result
}

pub fn cmd_look(token: Token, server: &mut Server)
{
    let vision = build_look(token, server);
    let response = format_look_response(server, &vision);

    debug_look_coordinates(token, &vision);
    debug_look_response(token, &response);

    send_result(token, server, &response);
}