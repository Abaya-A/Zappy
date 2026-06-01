/*
** Project  -  Zappy
** Date     -  May 27, 2026
**
** Copyright (c) 2026 Léo Lacordaire
*/

mod parser;
mod utils;

fn main()
{
    let params = parser::parse_args();
    println!("port = {}", params.port);
    println!("width = {}", params.width);
    println!("height = {}", params.height);
    println!("clients = {}", params.port);
    println!("frequency = {}", params.frequency);
    for i in params.teams {
        println!("teams = {}", i);
    }
}
