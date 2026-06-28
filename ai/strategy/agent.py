import random

from ai.strategy.parser import *
from ai.strategy.state import State
from ai.strategy.actions import *

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
            wait_incantation()
            return

        if goal == "GATHER_PLAYERS":
            follow_broadcast()
            return
        
        if current_tile:
            collect_current_tile()
            return

        if goal == "FORK":
            fork()
            return

        if goal.startswith("SEARCH_"):
            resource = goal.replace("SEARCH_", "").lower()
            search_resource(resource)
            return

        if goal == "LEVEL_UP":
            level_up()
            return

        self.client.command("Forward")