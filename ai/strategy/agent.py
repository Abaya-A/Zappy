import random
import re

from ai.strategy.state import State
from ai.strategy.parser import *
from ai.strategy.vision import *
from ai.strategy.levels import *


class Agent:

    def __init__(self, client):
        self.client = client
        self.state = State()

        self.state.role = random.choice(["WORKER", "COLLECTOR"])
        self.is_leader = False

    def run(self):
        print("[AI] Agent started...")

        while True:
            self.observe()
            self.think()
            self.act()

            if self.state.fork_cooldown > 0:
                self.state.fork_cooldown -= 1

    def observe(self):
        self.sync_level_from_client()

        raw_inventory = self.client.command("Inventory")
        self.sync_level_from_client()
        self.state.inventory = parse_inventory(raw_inventory)

        raw_look = self.client.command("Look")
        self.sync_level_from_client()
        self.state.visible_tiles = parse_look(raw_look)

        self.state.messages.extend(self.client.messages)
        self.client.messages.clear()

        print("Inventory:", self.state.inventory)
        print("Vision:", self.state.visible_tiles)
        print("Messages:", self.state.messages)

    def think(self):
        direction = self.get_latest_levelup_direction()

        if direction is not None:
            self.state.last_broadcast = direction
            self.state.current_goal = "FOLLOW_BROADCAST"
            return

        if self.state.ready_for_incantation():
            self.state.current_goal = "LEVEL_UP"
            return

        if self.state.food() < 10:
            self.state.current_goal = "SEARCH_FOOD"
            return

        if self.state.role == "COLLECTOR":
            resource = self.state.next_missing_resource()
            if resource:
                self.state.current_goal = f"SEARCH_{resource.upper()}"
                return

        if self.state.fork_cooldown <= 0 and self.state.food() > 20:
            self.state.current_goal = "FORK"
            return

        resource = self.state.next_missing_resource()
        if resource:
            self.state.current_goal = f"SEARCH_{resource.upper()}"
            return

        if self.state.missing_resources() == []:
            self.state.current_goal = "CALL_PLAYERS"
            return

        self.state.current_goal = "IDLE"

    def act(self):
        goal = self.state.current_goal

        if goal == "FOLLOW_BROADCAST":
            self.follow_broadcast()
            return

        if goal == "LEVEL_UP":
            self.level_up()
            return

        if goal == "CALL_PLAYERS":
            self.call_players()
            return

        if goal == "FORK":
            self.fork()
            return

        if goal and goal.startswith("SEARCH_"):
            resource = goal.replace("SEARCH_", "").lower()
            self.search_resource(resource)
            return

        current_tile = self.state.current_tile()

        if current_tile:
            self.collect_current_tile()
            return

        self.client.command("Forward")

    # CLIENT/EVENTS
    def sync_level_from_client(self):
        line = getattr(self.client, "current_level_line", None)

        if not line:
            return

        match = re.search(r"Current level:\s*(\d+)", line)

        if match:
            self.state.level = int(match.group(1))
            self.state.current_goal = None
            self.state.messages.clear()
            self.client.messages.clear()
            print("LEVEL UPDATED:", self.state.level)

        self.client.current_level_line = None

    def get_latest_levelup_direction(self):
        latest_direction = None
        had_levelup_message = False

        for msg in self.state.messages:
            direction, content = parse_broadcast(msg)

            if not content.startswith("LEVELUP:"):
                continue

            had_levelup_message = True

            try:
                target_level = int(content.split(":")[1])
            except ValueError:
                continue

            if target_level != self.state.level:
                continue

            latest_direction = direction

        if had_levelup_message:
            self.state.messages.clear()
            self.client.messages.clear()

        return latest_direction

    ## MOVEMENTS
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

    def follow_broadcast(self):
        direction = self.state.last_broadcast
        print(f"Following broadcast direction {direction}")

        if direction == 0:
            print("Already on leader tile")
            self.client.command("Look")
            self.state.current_goal = None
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

        self.state.current_goal = None
    
    ##RESOURCES 
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

        for resource in list(current_tile):
            if resource not in RESOURCE:
                continue

            print(f"Taking {resource}")
            self.client.command(f"Take {resource}")

    def clean_tile_before_incantation(self):
        current_tile = self.state.current_tile()

        for item in list(current_tile):
            if item in RESOURCE:
                print(f"Cleaning tile: taking {item}")
                self.client.command(f"Take {item}")

    ##INCANTATION
    def call_players(self):
        requirements = self.state.requirements()
        required_players = requirements.get("players", 1)
        players_here = self.state.player_count_on_tile()

        print(f"Players here: {players_here}/{required_players}")

        if players_here > required_players:
            print("Too many players here, moving away")
            self.client.command("Forward")
            self.state.current_goal = None
            return

        if players_here == required_players:
            print("Enough players, trying level up")
            self.state.current_goal = "LEVEL_UP"
            return

        if self.state.broadcast_cooldown <= 0:
            print("Calling players for incantation")
            self.client.command(f"Broadcast LEVELUP:{self.state.level}")
            self.state.broadcast_cooldown = 5
        else:
            self.state.broadcast_cooldown -= 1

        self.client.command("Look")

    def level_up(self):
        requirements = self.state.requirements()
        required_players = requirements.get("players", 1)
        players_here = self.state.player_count_on_tile()

        if players_here != required_players:
            print(f"Abort incantation: players {players_here}/{required_players}")
            self.state.current_goal = None
            return

        if not self.state.ready_for_incantation():
            self.state.current_goal = None
            return

        self.clean_tile_before_incantation()

        for resource, amount in requirements.items():
            if resource == "players":
                continue

            for _ in range(amount):
                self.client.command(f"Set {resource}")

        self.client.command(f"Broadcast LEVELUP:{self.state.level}")

        response = self.client.command("Incantation")

        self.apply_incantation_result(response)

        print("Incantation:", response)

    def apply_incantation_result(self, response):
        match = re.search(r"Current level:\s*(\d+)", response)

        if match:
            self.state.level = int(match.group(1))
            self.state.current_goal = None
            self.state.messages.clear()
            self.client.messages.clear()
            self.client.current_level_line = None
            print("LEVEL UPDATED:", self.state.level)
            return

        if response == "ko":
            self.state.current_goal = None
            self.state.messages.clear()
            self.client.messages.clear()
            print("INCANTATION FAILED")
            return
        
    #FORK
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
        self.state.current_goal = None