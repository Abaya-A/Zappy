/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;

use crate::utils::{
    compute_direction,
    notify_gui,
    send_response,
    send_result,
    Server,
};

fn broadcast_to_others(token: Token, server: &mut Server, message: &str, emitter_x: u32, emitter_y: u32)
{
    let width = server.params.width;
    let height = server.params.height;

    for (other_token, client) in server.clients.iter_mut() {
        if *other_token == token || client.is_gui {
            continue;
        }

        let Some(player) = &client.player else {
            continue;
        };

        let direction = compute_direction(
            emitter_x,
            emitter_y,
            player.x,
            player.y,
            &player.direction,
            width,
            height,
        );

        let response = format!("message {}, {}\n", direction, message);
        let _ = send_response(&mut client.stream, &response);
    }
}

fn notify_broadcast_to_gui(token: Token, server: &mut Server, message: &str)
{
    let player_number = token.0 as u32;
    let response = format!("pbc #{} {}\n", player_number, message);

    notify_gui(&mut server.clients, &response);
}

pub fn cmd_broadcast(token: Token, server: &mut Server, message: String)
{
    let (emitter_x, emitter_y) = {
        let client = server.clients.get(&token).unwrap();
        let player = client.player.as_ref().unwrap();

        (player.x, player.y)
    };

    broadcast_to_others(token, server, &message, emitter_x, emitter_y);
    send_result(token, server, "ok");
    notify_broadcast_to_gui(token, server, &message);
}