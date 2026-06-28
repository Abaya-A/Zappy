from ai.strategy.vision import tile_to_position

def move_to_tile(self, tile_index):
    x, y = tile_to_position(tile_index)

    if y <= 0:
        return

    if x < 0:
        self.client.command("Left")
        for _ in range(abs(x)):
            self.client.command("Forward")
        self.client.command("Right")

    elif x > 0:
        self.client.command("Right")
        for _ in range(abs(x)):
            self.client.command("Forward")
        self.client.command("Left")

    for _ in range(y):
        self.client.command("Forward")

def search_resource(self, resource):

    current_tile = self.state.current_tile()

    if resource in current_tile:
        print(f"Taking {resource}")
        self.client.command(f"Take {resource}")
        return

    tile_index = self.state.find_resource(resource)

    if tile_index is not None:
        print(f"Moving to {resource} at tile {tile_index}")
        self.move_to_tile(tile_index)
        return

    self.client.command("Forward")

def collect_current_tile(self):
    current_tile = self.state.current_tile()

    for resource in current_tile:
        print(f"Taking {resource}")
        self.client.command(f"Take {resource}")

def level_up(self):
    if not self.state.ready_for_incantation():
        return

    requirements = self.state.requirements()

    for resource, amount in requirements.items():
        if resource == "players":
            continue

    for _ in range(amount):
        self.client.command(f"Set {resource}")

    self.client.command("Broadcast LEVELUP")

    response = self.client.command("Incantation")
    if "Current level:" in response:
        self.state.level += 1
        self.state.is_waiting_incantation = False
        self.state.broadcast_cooldown = 0

    print("Incantation:", response)

def call_players(self):
    required_players = self.state.requirements().get("players", 1)
    players_here = self.state.player_count_on_tile()

    print(f"Players here: {players_here}/{required_players}")

    if self.state.broadcast_cooldown <= 0:
        print("Calling players for incantation")
        self.client.command("Broadcast LEVELUP")
        self.state.broadcast_cooldown = 5
    else:
        self.state.broadcast_cooldown -= 1

    self.client.command("Look")

def follow_broadcast(self):
    direction = self.state.last_broadcast
    print(f"Following broadcast direction {direction}")

    if direction == 0:
        print("Arrived at leader, waiting")
        self.state.is_waiting_incantation = True
        self.client.command("Look")
        return

    if direction == 1:
        self.client.command("Forward")
    elif direction in [2, 3, 4]:
        self.client.command("Left")
        self.client.command("Forward")
    elif direction == 5:
        self.client.command("Left")
        self.client.command("Left")
        self.client.command("Forward")
    elif direction in [6, 7, 8]:
        self.client.command("Right")
        self.client.command("Forward")
    
def wait_incantation(self):
    print("Waiting for incantation")
    self.client.command("Look")

def fork(self):

    print("Checking fork possibility")

    response = self.client.command("Connect_nbr")
    print("Connect_nbr:", response)

    try:
        slots = int(response)
    except ValueError:
        self.state.fork_cooldown = 5
        return
    
    if slots > 0:
        print("Forking")
        self.client.command("Fork")
    else:
        print("No available team slot")

    self.state.fork_cooldown = 20
