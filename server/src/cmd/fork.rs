/*
 *  Project  -  cmd
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::Token;

use crate::utils::{
    notify_gui,
    send_result,
    Egg,
    Server,
};

fn get_fork_context(token: Token, server: &Server) -> (u32, u32, String)
{
    let client = server.clients.get(&token).unwrap();
    let player = client.player.as_ref().unwrap();
    let team = client.team_name.clone().unwrap();

    (player.x, player.y, team)
}

fn create_egg(server: &mut Server, team: String, x: u32, y: u32) -> u32
{
    let egg_id = server.world.eggs.len() as u32;

    server.world.eggs.push(Egg { team, x, y });

    egg_id
}

fn notify_fork_to_gui(token: Token, server: &mut Server, egg_id: u32, x: u32, y: u32)
{
    let player_number = token.0 as u32;

    notify_gui(&mut server.clients, &format!("pfk #{}\n", player_number));
    notify_gui(
        &mut server.clients,
        &format!("enw #{} #{} {} {}\n", egg_id, player_number, x, y),
    );
}

pub fn cmd_fork(token: Token, server: &mut Server)
{
    let (x, y, team) = get_fork_context(token, server);
    let egg_id = create_egg(server, team, x, y);

    send_result(token, server, "ok");
    notify_fork_to_gui(token, server, egg_id, x, y);
}