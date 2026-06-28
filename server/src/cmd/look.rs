/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Server, Direction, send_result};
use crate::timers;

fn get_player(token: Token, server: &Server) -> (u32, u32, Direction, u32)
{
    let client = server.clients.get(&token).unwrap();
    let player = client.player.as_ref().unwrap();

    (player.x, player.y, player.direction.clone(), player.level)
}

fn get_line_of_view(server: &Server, x: u32, y: u32, dir: Direction, distance: u32) -> Vec<(u32, u32)>
{
    let mut tiles = Vec::new();

    let width = server.params.width;
    let height = server.params.height;

    for i in 0..(distance * 2 + 1) {
        let (tx, ty) = match dir {
            Direction::N => ((x + i + width - distance) % width, (y + height - distance) % height),
            Direction::S => ((x + distance + width - i) % width, (y + distance) % height),
            Direction::E => ((x + distance) % width, (y + i + height - distance) % height),
            Direction::W => ((x + width - distance) % width, (y + distance + height - i) % height),
        };

        tiles.push((tx, ty));
    }

    tiles
}

fn build_look(token: Token, server: &Server) -> Vec<Vec<(u32, u32)>>
{
    let (x, y, dir, level) = get_player(token, server);

    let mut tiles = Vec::new();

    let mut distance = 0;

    loop {
        let line = get_line_of_view(server, x, y, dir, distance);
        tiles.push(line);

        distance += 1;

        if distance > level {
            break;
        }
    }

    tiles
}

fn format_str(server: &Server, vision: &Vec<Vec<(u32, u32)>>) -> String
{
    let mut result = String::from("[");

    let mut first = true;

    for line in vision {
        for (x, y) in line {
            if !first {
                result.push_str(", ");
            }
            first = false;

            let mut items = String::new();

            // add player
            for client in server.clients.values() {
                if let Some(player) = &client.player {
                    if player.x == *x && player.y == *y {
                        if !items.is_empty() {
                            items.push(' ');
                        }
                        items.push_str("player");
                    }
                }
            }

            // add resources
            let tile = &server.world.tiles[*y as usize][*x as usize];
            for _ in &tile.players {
                if !items.is_empty() {
                    items.push(' ');
                }
                items.push_str("player");
            }
            for (res, count) in &tile.resources {
                for _ in 0..*count {
                    if !items.is_empty() {
                        items.push(' ');
                    }
                    items.push_str(res);
                }
            }

            result.push_str(&items);
        }
    }

    result.push(']');
    result
}

pub fn cmd_look(token: Token, server: &mut Server)
{
    let looking = build_look(token, server);
    let response = format_str(server, &looking);

    send_result(token, server, &response);
    timers::start_action(token, server, 7);
}
