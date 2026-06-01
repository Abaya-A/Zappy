/*
** Project  -  Zappy
** Date     -  May 27, 2026
**
** Copyright (c) 2026 Léo Lacordaire
*/

use std::env;
use std::process;
use crate::utils::ServerParams;

// check si il manque un params
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
        println!("[ERROR]: missing arguments: {} or --help for usage.", missing.join(", "));
        std::process::exit(84);
    }
}

fn wong_data(flag: &str) -> !
{
    if flag == "-n" {
        eprintln!("missing teams");
    } else {
        eprintln!("bad {}", flag);
    }
    process::exit(84)
}

pub fn parse_args() -> ServerParams
{
    let args: Vec<String> = env::args().collect();
    let mut iter = args.iter().skip(1).peekable();
    let mut params = ServerParams {
        port: 0,
        width: 0,
        height: 0,
        teams: Vec::new(),
        clients: 0,
        frequency: 0,
    };

    if args[0] == "./zappy_server" || args[1] == "--help" {
        println!("USAGE: ./zappy_server -p port -x width -y height -n name1 name2 ... -c clientsNb -f freq");
        return params;
    }

    // pour pas send les vrais args sinon ça pet
    missing_argument(args.clone());

    while let Some(arg) = iter.next() {
        match arg.as_str() {
            "-p" => params.port = iter.next().and_then(|v| v.parse().ok()).unwrap_or_else(|| wong_data("-p")),
            "-x" => params.width = iter.next().and_then(|v| v.parse().ok()).unwrap_or_else(|| wong_data("-x")),
            "-y" => params.height = iter.next().and_then(|v| v.parse().ok()).unwrap_or_else(|| wong_data("-y")),
            "-c" => params.clients = iter.next().and_then(|v| v.parse().ok()).unwrap_or_else(|| wong_data("-c")),
            "-f" => params.frequency = iter.next().and_then(|v| v.parse().ok()).unwrap_or_else(|| wong_data("-f")),
            "-n" => {
                    // peek check le params d'après pour voir si c'est un flag ou non 
                    while let Some(team) = iter.peek() {
                        if team.starts_with('-') {
                            break;
                        }
                        params.teams.push(iter.next().unwrap().to_string());
                    }
                },
            _ => { eprintln!("unknown arg: {}", arg); process::exit(84) }
        }
    }

    return params;
}
