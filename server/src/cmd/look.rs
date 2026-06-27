/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::utils::{Server, Direction, send_result};
use crate::timers;

fn get_player(token: Token, server: &Server) -> (u32, u32, Direction)
{
    let client = server.clients.get(&token).unwrap();
    let player = client.player.as_ref().unwrap();

    (player.x, player.y, player.direction.clone())
}

fn get_line_of_view(server: &Server, x: u32, y: u32, dir: Direction, distance: u32) -> Vec<(u32, u32)>
{
    let mut tiles = Vec::new();

    let width = server.params.width;
    let height = server.params.height;

    for i in 0..(distance * 2 + 1) {
        let (tx, ty) = match dir {
            Direction::N => ((x + i + width - distance) % width, (y + height - distance - 1) % height),
            Direction::S => ((x + i + width - distance) % width, (y + distance + 1) % height),
            Direction::E => ((x + distance + 1) % width, (y + i + height - distance) % height),
            Direction::W => ((x + width - distance - 1) % width, (y + i + height - distance) % height),
        };

        tiles.push((tx, ty));
    }

    tiles
}

fn build_look(token: Token, server: &Server) -> Vec<Vec<(u32, u32)>>
{
    let (x, y, dir) = get_player(token, server);

    let mut tiles = Vec::new();

    let mut distance = 0;

    loop {
        let line = get_line_of_view(server, x, y, dir, distance);
        tiles.push(line);

        distance += 1;

        if distance > 3 {
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
            let tile = &server.world.tiles[*y as usize][*x as usize];
            let mut items = String::new();
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
    if !timers::can_act(token, server) {
        send_result(token, server, "ko");
        return;
    }

    let looking = build_look(token, server);
    let response = format_str(server, &looking);

    send_result(token, server, &response);
    timers::start_action(token, server, 7);
}
