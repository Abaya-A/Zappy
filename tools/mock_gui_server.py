#!/usr/bin/env python3

import socket
import time

HOST = "127.0.0.1"
PORT = 4242

messages = [
    "msz 10 10\n",
    "sgt 100\n",
    "tna teamA\n",
    "tna teamB\n",

    # Single-resource tiles on bottom row
    "bct 0 0 1 0 0 0 0 0 0\n",  # food
    "bct 1 0 0 1 0 0 0 0 0\n",  # linemate
    "bct 2 0 0 0 1 0 0 0 0\n",  # deraumere
    "bct 3 0 0 0 0 1 0 0 0\n",  # sibur
    "bct 4 0 0 0 0 0 1 0 0\n",  # mendiane
    "bct 5 0 0 0 0 0 0 1 0\n",  # phiras
    "bct 6 0 0 0 0 0 0 0 1\n",  # thystame

    # All resources on one tile
    "bct 1 2 1 1 1 1 1 1 1\n",

    # Multiple quantities, should still draw one marker per resource type for now
    "bct 4 4 5 2 3 1 4 2 1\n",

    # Players
    "pnw #1 2 3 1 1 teamA\n",  # player #1 starts level 1, north
    "pnw #2 5 6 2 2 teamB\n",  # player #2 starts level 2, east

    # Egg
    "enw #1 #1 4 4\n",

    # Inventory
    "pin #1 2 3 10 1 0 0 0 0 0\n",

    # Move player #1, then level him up after a visible delay
    "ppo #1 3 3 2\n",          # player #1 moves east
    "pbc #1 watch me level up\n",
    "plv #1 5\n",              # player #1 becomes visibly larger

    "pbc #1 hello from mock server\n",
]

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind((HOST, PORT))
    server.listen(1)

    print(f"Mock GUI server listening on {HOST}:{PORT}")
    conn, addr = server.accept()

    with conn:
        print(f"Client connected: {addr}")

        conn.sendall(b"WELCOME\n")

        data = conn.recv(4096)
        print("Received from GUI:")
        print(data.decode(errors="replace"))

        for message in messages:
            print("->", message.strip())
            conn.sendall(message.encode())
            time.sleep(0.3)

        while True:
            time.sleep(1)