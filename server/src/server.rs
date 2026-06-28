/*
 *  Project  -  src
 *  Date     -  June 15th 2026
 *
 *  Copyright (c) 2026 Jules Nourdin
 */

use mio::net::TcpListener;
use mio::{Events, Interest, Poll, Token};

use std::collections::{HashMap, VecDeque};
use std::net::{Shutdown, SocketAddr};
use std::time::{SystemTime, UNIX_EPOCH};

use crate::handle_client;
use crate::timers;
use crate::timers::{get_poll_timeout, verify_player_hunger};
use crate::utils::{self, Server, ServerParams, World};

const SERVER_TOKEN: Token = Token(0);
const EVENTS_CAPACITY: usize = 128;

const RESOURCE_DENSITIES: [(&str, f64); 7] = [
    ("food", 0.5),
    ("linemate", 0.3),
    ("deraumere", 0.15),
    ("sibur", 0.1),
    ("mendiane", 0.1),
    ("phiras", 0.08),
    ("thystame", 0.05),
];

pub fn start_server(params: ServerParams)
{
    let mut poll = Poll::new().unwrap();
    let mut events = Events::with_capacity(EVENTS_CAPACITY);
    let mut server = init_world(&params);
    let mut next_token = 1;

    let mut server_socket = bind_server_socket(params.port);
    register_server_socket(&poll, &mut server_socket);

    loop {
        poll_server_events(&mut poll, &mut events, &mut server);
        handle_polled_events(
            &poll,
            &mut server_socket,
            &mut server,
            &mut next_token,
            &events,
        );
        update_server_state(&mut server);
    }
}

fn bind_server_socket(port: u16) -> TcpListener
{
    let address = SocketAddr::from(([0, 0, 0, 0], port));

    TcpListener::bind(address).unwrap_or_else(|_| {
        println!("Port already in use!");
        std::process::exit(84);
    })
}

fn register_server_socket(poll: &Poll, server_socket: &mut TcpListener)
{
    poll.registry()
        .register(server_socket, SERVER_TOKEN, Interest::READABLE)
        .unwrap();
}

fn poll_server_events(poll: &mut Poll, events: &mut Events, server: &mut Server)
{
    let timeout = get_poll_timeout(server);

    if let Err(error) = poll.poll(events, timeout) {
        if error.kind() == std::io::ErrorKind::Interrupted {
            return;
        }

        eprintln!("Poll error: {}", error);
        std::process::exit(84);
    }
}

fn handle_polled_events(
    poll: &Poll,
    server_socket: &mut TcpListener,
    server: &mut Server,
    next_token: &mut usize,
    events: &Events,
) {
    for event in events.iter() {
        match event.token() {
            SERVER_TOKEN => accept_client(poll, server_socket, server, next_token),
            token => handle_client_event(token, server),
        }
    }
}

fn accept_client(
    poll: &Poll,
    server_socket: &mut TcpListener,
    server: &mut Server,
    next_token: &mut usize,
) {
    let Ok((mut client_socket, client_addr)) = server_socket.accept() else {
        return;
    };

    let token = Token(*next_token);
    *next_token += 1;

    poll.registry()
        .register(&mut client_socket, token, Interest::READABLE)
        .unwrap();

    if let Err(error) = utils::send_response(&mut client_socket, "WELCOME\n") {
        eprintln!("Failed to send WELCOME: {}", error);
    }

    server.clients.insert(token, create_client(client_socket));
    println!("New client: {}", client_addr);
}

fn create_client(client_socket: mio::net::TcpStream) -> utils::Client
{
    utils::Client {
        stream: client_socket,
        buffer: String::new(),
        team_name: None,
        player: None,
        is_gui: false,
        action_deadline: None,
        hunger_check_deadline: timers::get_deadline(100),
        command_queue: VecDeque::new(),
        active_command: None,
    }
}

fn handle_client_event(token: Token, server: &mut Server)
{
    if !server.clients.contains_key(&token) {
        return;
    }

    if let Err(error) = handle_client::handle_client(token, server) {
        eprintln!("{}", error);
        disconnect_client(token, server);
    }
}

fn disconnect_client(token: Token, server: &mut Server)
{
    let Some(client) = server.clients.remove(&token) else {
        return;
    };

    restore_team_slot(server, &client);
    remove_player_from_tile(token, server, &client);

    let _ = client.stream.shutdown(Shutdown::Both);
}

fn restore_team_slot(server: &mut Server, client: &utils::Client)
{
    let Some(team_name) = &client.team_name else {
        return;
    };

    if client.is_gui {
        return;
    }

    let Some(team) = server
        .teams
        .iter_mut()
        .find(|team| team.name == *team_name) else {
            return;
        };

    team.available_slots += 1;
}

fn remove_player_from_tile(token: Token, server: &mut Server, client: &utils::Client)
{
    let Some(player) = &client.player else {
        return;
    };

    let tile = &mut server.world.tiles[player.y as usize][player.x as usize];
    tile.players.retain(|player_token| *player_token != token);
}

fn update_server_state(server: &mut Server)
{
    verify_player_hunger(server);
    timers::process_queued_commands(server);
}

fn init_world(params: &ServerParams) -> Server
{
    let tiles = create_tiles(params.width, params.height);
    let eggs = Vec::new();
    let teams = create_teams(params);

    let mut server = Server {
        clients: HashMap::new(),
        params: params.clone(),
        world: World { tiles, eggs },
        teams,
    };

    spawn_resources(&mut server);
    server
}

fn create_tiles(width: u32, height: u32) -> Vec<Vec<utils::Tile>>
{
    let mut tiles = Vec::new();

    for _ in 0..height {
        let mut row = Vec::new();

        for _ in 0..width {
            row.push(utils::Tile {
                players: Vec::new(),
                resources: create_empty_resources(),
            });
        }

        tiles.push(row);
    }

    tiles
}

fn create_empty_resources() -> HashMap<String, u32>
{
    let mut resources = HashMap::new();

    for &(resource, _) in &RESOURCE_DENSITIES {
        resources.insert(resource.to_string(), 0);
    }

    resources
}

fn create_teams(params: &ServerParams) -> Vec<utils::Team>
{
    params
        .teams_names
        .iter()
        .map(|name| utils::Team {
            name: name.clone(),
            available_slots: params.team_clients_nb as usize,
        })
        .collect()
}

fn current_time_seed() -> u64
{
    SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_nanos() as u64
}

fn next_random(state: &mut u64) -> u64
{
    *state = state
        .wrapping_mul(6364136223846793005)
        .wrapping_add(1442695040888963407);

    *state >> 32
}

fn random_coord(state: &mut u64, max: u32) -> u32
{
    (next_random(state) % max as u64) as u32
}

fn spawn_resources(server: &mut Server)
{
    let width = server.params.width;
    let height = server.params.height;

    if width == 0 || height == 0 {
        return;
    }

    let total_tiles = width * height;
    let mut rng_state = current_time_seed();

    for &(resource, density) in &RESOURCE_DENSITIES {
        spawn_resource_type(
            server,
            &mut rng_state,
            resource,
            resource_quantity(total_tiles, density),
        );
    }
}

fn resource_quantity(total_tiles: u32, density: f64) -> u32
{
    ((total_tiles as f64) * density).ceil() as u32
}

fn spawn_resource_type(
    server: &mut Server,
    rng_state: &mut u64,
    resource: &str,
    quantity: u32,
) {
    for _ in 0..quantity {
        let x = random_coord(rng_state, server.params.width);
        let y = random_coord(rng_state, server.params.height);

        add_resource_to_tile(server, x, y, resource);
    }
}

fn add_resource_to_tile(server: &mut Server, x: u32, y: u32, resource: &str)
{
    *server.world.tiles[y as usize][x as usize]
        .resources
        .entry(resource.to_string())
        .or_insert(0) += 1;
}