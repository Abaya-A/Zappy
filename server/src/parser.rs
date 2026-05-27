/*
** Project  -  Zappy
** Date     -  May 27, 2026
**
** Copyright (c) 2026 Léo Lacordaire
*/

use std::env;

// check lequel manque pour le display
fn missing_argument(args: Vec<String>)
{
    let required = ["-p", "-x", "-y", "-n", "-c", "-f"];
    let mut missing = Vec::new();

    for &flag in &required {
        if let Some(pos) = args.iter().position(|s| s == flag) {
            if pos + 1 >= args.len() || args[pos + 1].starts_with('-') {
                missing.push(format!("{} (missing value)", flag));
            }
        } else {
            missing.push(flag.to_string());
        }
    }

    if !missing.is_empty() {
        println!("[ERROR]: missing arguments: {}", missing.join(", "));
        std::process::exit(84);
    }
}

pub fn parse_args()
{
    let args: Vec<String> = env::args().collect();

    if args.len() < 13 {
        missing_argument(args);
    }
}