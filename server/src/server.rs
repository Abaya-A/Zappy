/*
 *  Project  -  src
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
*/

use crate::handle_client;
use crate::timers::get_poll_timeout;
use crate::timers::verify_player_hunger;
use crate::utils::World;
use crate::utils::Server;
use crate::utils::ServerParams;
use crate::timers;

use mio::net::TcpListener;
use mio::{Events, Interest, Poll, Token};
use std::collections::HashMap;
use std::net::SocketAddr;

const SERVER_TOKEN: Token = Token(0);

pub fn start_server(params: ServerParams) {
    let mut poll = Poll::new().unwrap();
    let mut events = Events::with_capacity(128);

    let mut server = init_world(&params);

    let mut next_token = 1;

    let addr = SocketAddr::from(([0, 0, 0, 0], params.port));
    let mut server_socket = TcpListener::bind(addr).unwrap_or_else(|_| {
        println!("Port already in use!");
        std::process::exit(84);
    });

    poll.registry()
        .register(&mut server_socket, SERVER_TOKEN, Interest::READABLE)
        .unwrap();

    loop {
        let timeout = get_poll_timeout(&mut server);
        if let Err(e) = poll.poll(&mut events, timeout) {
            if e.kind() == std::io::ErrorKind::Interrupted {
                continue;
            }
            eprintln!("Poll error: {}", e);
            std::process::exit(84);
        }

        for event in events.iter() {
            match event.token() {
                SERVER_TOKEN => {
                    let (mut client_socket, client_addr) = server_socket.accept().unwrap();
                    let token = Token(next_token);
                    next_token += 1;
                    poll.registry()
                        .register(&mut client_socket, token, Interest::READABLE)
                        .unwrap();

                    if let Err(e) = crate::utils::send_response(&mut client_socket, "WELCOME\n") {
                        eprintln!("Failed to send WELCOME: {}", e);
                    }

                    let client = crate::utils::Client {
                        stream: client_socket,
                        buffer: String::new(),
                        team_name: None,
                        player: None,
                        is_gui: false,
                        action_deadline: None,
                        hunger_check_deadline: timers::get_deadline(100),
                    };

                    server.clients.insert(token, client);
                    println!("New client: {}", client_addr);
                }
                token => {
                    let client_exists = server.clients.contains_key(&token);
                    if !client_exists {
                        continue;
                    }
                    if let Err(e) = handle_client::handle_client(token, &mut server) {
                        eprintln!("{}", e);
                        if let Some(client) = server.clients.remove(&token) {
                            if let Some(team_name) = &client.team_name {
                                if let Some(team) =
                                    server.teams.iter_mut().find(|t| &t.name == team_name)
                                {
                                    team.available_slots += 1;
                                }
                            }
                            let _ = client.stream.shutdown(std::net::Shutdown::Both);
                        }
                    }
                }
            }
        }
        verify_player_hunger(&mut server);
    }
}

fn init_world(params: &ServerParams) -> Server
{
    let mut tiles = Vec::new();
    let eggs = Vec::new();
    for _ in 0..params.height {
        let mut row = Vec::new();
        for _ in 0..params.width {
            let mut resources = HashMap::new();
            resources.insert("food".to_string(), 0);
            resources.insert("linemate".to_string(), 0);
            resources.insert("deraumere".to_string(), 0);
            resources.insert("sibur".to_string(), 0);
            resources.insert("mendiane".to_string(), 0);
            resources.insert("phiras".to_string(), 0);
            resources.insert("thystame".to_string(), 0);
            row.push(crate::utils::Tile {
                players: Vec::new(),
                resources,
            });
        }
        tiles.push(row);
    }

    let teams = params
        .teams_names
        .iter()
        .map(|name| crate::utils::Team {
            name: name.clone(),
            available_slots: params.team_clients_nb as usize,
        })
        .collect();

    let mut server = Server {
        clients: HashMap::new(),
        params: params.clone(),
        world: World { tiles, eggs },
        teams,
    };

    spawn_resources(&mut server);
    server
}


fn spawn_resources(server: &mut Server) {
    let width = server.params.width;
    let height = server.params.height;
    if width == 0 || height == 0 {
        return;
    }
    let total_tiles = width * height;

    let densities = [
        ("food", 0.5f64),
        ("linemate", 0.3f64),
        ("deraumere", 0.15f64),
        ("sibur", 0.1f64),
        ("mendiane", 0.1f64),
        ("phiras", 0.08f64),
        ("thystame", 0.05f64),
    ];

    let mut rng_state = 42u32;
    let mut lcg = || {
        rng_state = rng_state.wrapping_mul(1103515245).wrapping_add(12345);
        rng_state & 0x7fffffff
    };

    for &(resource, density) in &densities {
        let total_qty = ((total_tiles as f64) * density).ceil() as u32;
        for _ in 0..total_qty {
            let rx = lcg() % width;
            let ry = lcg() % height;
            let count = server.world.tiles[ry as usize][rx as usize]
                .resources
                .entry(resource.to_string())
                .or_insert(0);
            *count += 1;
        }
    }
}
