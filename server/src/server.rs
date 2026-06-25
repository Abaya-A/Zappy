/*
 *  Project  -  src
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
*/

use crate::handle_client;
use crate::utils::World;
use crate::utils::Server;
use crate::utils::ServerParams;

use mio::net::TcpListener;
use mio::{Events, Interest, Poll, Token};
use std::collections::HashMap;
use std::net::SocketAddr;

const SERVER_TOKEN: Token = Token(0);

pub fn start_server(params: ServerParams) {
    let mut poll = Poll::new().unwrap();
    let mut events = Events::with_capacity(128);
    let mut server = Server { clients: HashMap::new(), params: params.clone(), world: World {tiles: Vec::new(),}, };

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
        if let Err(e) = poll.poll(&mut events, None) {
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
                    };

                    server.clients.insert(token, client);
                    println!("New client: {}", client_addr);
                }
                token => {
                    if let Err(e) = handle_client::handle_client(token, &mut server) {
                        eprintln!("{}", e);
                        std::process::exit(84);
                    }
                }
            }
        }
    }
}
