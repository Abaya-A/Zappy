/*
 *  Project  -  src
 *  Date     -  June 29th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;
use crate::types::game::Resource;
use crate::types::network::Server;


// Handle client
pub fn debug_queue_received(token: Token, is_gui: bool, queue_len: usize, request: &str)
{
    println!(
        "[QUEUE] token={:?} is_gui={} len={} cmd={}",
        token,
        is_gui,
        queue_len,
        request
    );
}

pub fn debug_queue_drop(token: Token, request: &str)
{
    println!(
        "[QUEUE DROP] token={:?} cmd={}",
        token,
        request
    );
}

pub fn debug_handshake(token: Token, team: &str)
{
    println!(
        "[HANDSHAKE] token={:?} team={}",
        token,
        team
    );
}


// Timer
pub fn debug_execute_command(token: Token, is_gui: bool, command: &str)
{
    println!(
        "[EXEC] token={:?} is_gui={} cmd={}",
        token,
        is_gui,
        command
    );
}

pub fn debug_consumed_food(any_food_consumed: bool, server: &mut Server)
{
    if any_food_consumed {
        let total_food: u32 = server.world.tiles.iter()
            .flat_map(|row| row.iter())
            .map(|tile| tile.resources.get(Resource::Food))
            .sum();
        println!("[FOOD] Food remaining on map: {}", total_food);
    }
}


// Look
type Position = (u32, u32);

pub fn debug_look_coordinates(token: Token, vision: &[Vec<Position>])
{
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

pub fn debug_look_response(token: Token, response: &str)
{
    println!("[LOOK RESPONSE] token={:?} {}", token, response);
}
