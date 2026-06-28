import random
import re

from ai.strategy.state import State
from ai.strategy.parser import *
from ai.strategy.vision import *
class Agent:

    def __init__(self, client):
        self.client = client
        self.state = State()
        self.state.role = random.choice(["WORKER", "COLLECTOR"])

    def run(self):

        print("[AI] Agent started...")

        while True:
            self.observe()
            self.think()
            self.act()

            if self.state.fork_cooldown > 0:
                self.state.fork_cooldown -= 1

    def observe(self):

        raw_inventory = self.client.command("Inventory")
        self.state.inventory = parse_inventory(raw_inventory)

        raw_look = self.client.command("Look")
        self.state.visible_tiles = parse_look(raw_look)

        self.state.messages.extend(self.client.messages)
        self.client.messages.clear()

        print("Inventory:", self.state.inventory)
        print("Vision:", self.state.visible_tiles)
        print("Messages:", self.state.messages)

    def think(self):
        
        #Lire les messages
        for msg in self.state.messages:
            direction, content = parse_broadcast(msg)
            
            if content == "LEVELUP" and direction != 0:
                self.state.current_goal = "GATHER_PLAYERS"
                self.state.last_broadcast = direction
                self.state.messages.clear()
                return

        #Attend une incantation
        if self.state.is_waiting_incantation:
            self.state.current_goal = "WAIT_INCANTATION"
            return

        #Cherche nourriture si nécessaire
        if self.state.food() < 10:
            self.state.current_goal = "SEARCH_FOOD"
            return

        #Rôle collector
        if self.state.role == "COLLECTOR":
            resource = self.state.next_missing_resource()
            if resource:
                self.state.current_goal = f"SEARCH_{resource.upper()}"

        #Fork
        if self.state.fork_cooldown <= 0 and self.state.food() > 20:
            self.state.current_goal = "FORK"
            return

        #Ressources manquantes
        resource = self.state.next_missing_resource()
        if resource:
            self.state.current_goal = f"SEARCH_{resource.upper()}"
            return

        #Incantation
        required_players = self.state.requirements().get("players", 1)
        players_here = self.state.player_count_on_tile()

        if players_here < required_players:
            self.state.current_goal = "CALL_PLAYERS"
            return

        self.state.current_goal = "LEVEL_UP"
        print("Goal:", self.state.current_goal)

    def act(self):
        current_tile = self.state.current_tile()
        goal = self.state.current_goal

        if goal == "WAIT_INCANTATION":
            self.wait_incantation()
            return

        if goal == "GATHER_PLAYERS":
            self.follow_broadcast()
            return
        
        if current_tile:
            self.collect_current_tile()
            return

        if goal == "FORK":
            self.fork()
            return

        if goal.startswith("SEARCH_"):
            resource = goal.replace("SEARCH_", "").lower()
            self.search_resource(resource)
            return

        if goal == "LEVEL_UP":
            self.level_up()
            return

        self.client.command("Forward")

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

        match = re.search(r"Current level:\s*(\d+)", response)
        if match:
            self.state.level = int(match.group(1))
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
