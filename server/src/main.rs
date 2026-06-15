/*
** Project  -  Zappy
** Date     -  May 27, 2026
**
** Copyright (c) 2026 Léo Lacordaire
*/

mod server;
mod parser;
mod utils;
mod handle_client;
mod handle_command;
mod cmd;

fn main()
{
    let params = parser::parse_args();

    // Handle Ctrl+C event
    ctrlc::set_handler(|| {
        println!("\nShutting down...");
        std::process::exit(0);
    }).unwrap();

    if params.is_err() { std::process::exit(84); }
    server::start_server(params.unwrap());
}
