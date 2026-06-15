/*
** Project  -  Zappy
** Date     -  Jun 1, 2026
**
** Copyright (c) 2026 Léo Lacordaire
*/

use std::collections::HashMap;
use mio::Token;
use mio::net::TcpStream;
use std::io::Write;

#[derive(Clone)]
pub struct ServerParams
{
    pub port: u16,
    pub width: u32,
    pub height: u32,
    pub teams_names: Vec<String>,
    pub team_clients_nb: u32,
    pub frequency: u32
}

pub struct Client
{
    pub stream: TcpStream,
    pub buffer: String,
    pub team_name: Option<String>,
}

pub struct Server
{
    pub clients: HashMap<Token, Client>,
    pub params: ServerParams
}

pub fn send_response(stream: &mut TcpStream, msg: &str) -> std::io::Result<()>
{
    stream.write_all(msg.as_bytes())
}
