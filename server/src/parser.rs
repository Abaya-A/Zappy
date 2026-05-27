/*
** Project  -  Zappy
** Date     -  May 27, 2026
**
** Copyright (c) 2026 Léo Lacordaire
*/

use std::env;

// handle les args manquant et check lequel manque pour le display
fn missing_argument()
{
    println!("[ERROR]: missing arguments");
    std::process::exit(84);
}

pub fn parse_args()
{
    println!("Parser");

    let args: Vec<String> = env::args().collect();

    println!("{:?}", args);

    if args.len() < 13 {
        missing_argument();
    }
}