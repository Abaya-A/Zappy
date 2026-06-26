#!/usr/bin/env python3

import socket
import time

HOST = "127.0.0.1"
PORT = 4242

KEEP_EGGS_VISIBLE = False

def base_world_messages():
    return [
        "msz 10 10\n",
        "sgt 100\n",
        "tna teamA\n",
        "tna teamB\n",
    ]


def resource_messages():
    return [
        "bct 0 0 1 0 0 0 0 0 0\n",
        "bct 1 0 0 1 0 0 0 0 0\n",
        "bct 2 0 0 0 1 0 0 0 0\n",
        "bct 3 0 0 0 0 1 0 0 0\n",
        "bct 4 0 0 0 0 0 1 0 0\n",
        "bct 5 0 0 0 0 0 0 1 0\n",
        "bct 6 0 0 0 0 0 0 0 1\n",
        "bct 1 2 1 1 1 1 1 1 1\n",
        "bct 4 4 5 2 3 1 4 2 1\n",
    ]


def entity_messages():
    return [
        "pnw #1 2 3 1 1 teamA\n",
        "pnw #2 5 6 2 2 teamB\n",
        "enw #1 #1 4 4\n",
        "pin #1 2 3 10 1 0 0 0 0 0\n",
    ]


def gameplay_messages():
    return [
        "ppo #1 3 3 2\n",
        "pbc #1 watch me level up\n",
        "plv #1 5\n",
        "pbc #2 broadcast before death\n",
        "pic 3 3 2 #1 #2\n",
        "pie 3 3 1\n",
        "pex #1\n",
    ]


def egg_lifecycle_messages():
    if KEEP_EGGS_VISIBLE:
        return []

    return [
        "ebo #1\n",
    ]


def cleanup_messages():
    return [
        "pdi #2\n",
        "pbc #1 hello from mock server\n",
        "pfk #1\n",
        "pdr #1 0\n",
        "pgt #1 1\n",
        "sst 120\n",
        "smg server maintenance soon\n",
        "suc\n",
        "sbp\n",
        "seg teamA\n",
    ]


def scenario_messages():
    return (
        gameplay_messages()
        + egg_lifecycle_messages()
        + cleanup_messages()
    )


def message_delay(message):
    if message.startswith("pic "):
        return 2.0
    if message.startswith("pbc "):
        return 1.0
    if message.startswith("pex "):
        return 1.0
    if message.startswith("pfk "):
        return 0.8
    if message.startswith("pdr ") or message.startswith("pgt "):
        return 0.8
    if message.startswith("plv "):
        return 1.0
    if message.startswith("pie "):
        return 0.8
    if message.startswith("ebo ") or message.startswith("edi "):
        return 1.0
    if message.startswith("pdi "):
        return 1.0
    if message.startswith("seg "):
        return 2.0
    return 0.3


def build_messages():
    return (
        base_world_messages()
        + resource_messages()
        + entity_messages()
        + scenario_messages()
    )


def send_handshake(conn):
    conn.sendall(b"WELCOME\n")

    data = conn.recv(4096)
    print("Received from GUI:")
    print(data.decode(errors="replace"))


def send_messages(conn, messages):
    for message in messages:
        print("->", message.strip())
        conn.sendall(message.encode())
        time.sleep(message_delay(message))


def run_server():
    messages = build_messages()

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((HOST, PORT))
        server.listen(1)

        print(f"Mock GUI server listening on {HOST}:{PORT}")
        print(f"KEEP_EGGS_VISIBLE={KEEP_EGGS_VISIBLE}")

        conn, addr = server.accept()

        with conn:
            print(f"Client connected: {addr}")
            send_handshake(conn)
            send_messages(conn, messages)

            print("Scenario complete. Keeping connection open.")
            while True:
                time.sleep(1)


if __name__ == "__main__":
    run_server()