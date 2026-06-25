/*
** Project  -  Zappy
** Date     -  May 27, 2026
**
** Copyright (c) 2026 Léo Lacordaire
*/

mod cmd;
mod gui_cmd;
mod handle_client;
mod handle_command;
mod parser;
mod server;
mod utils;

fn main() {
    let params = parser::parse_args();

    // Handle Ctrl+C event
    ctrlc::set_handler(|| {
        println!("\nShutting down...");
        std::process::exit(0);
    })
    .unwrap();

    if params.is_err() {
        std::process::exit(84);
    }
    server::start_server(params.unwrap());
}
