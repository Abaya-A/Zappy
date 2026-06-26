from ai.strategy.parser import parse_inventory, parse_look
from ai.strategy.state import State

class Agent:

    def __init__(self, client):
        self.client = client
        self.state = State()

    def run(self):

        print("[AI] Agent started...")

        while True:
            self.observe()
            self.think()
            self.act()

    def observe(self):

        raw_inventory = self.client.command("Inventory")
        self.state.inventory = parse_inventory(raw_inventory)

        raw_look = self.client.command("Look")
        self.state.visible_tiles = parse_look(raw_look)

        print("Inventory:", self.state.inventory)
        print("Vision:", self.state.visible_tiles)

    def think(self):

        if self.state.food() < 5:
            self.state.current_goal = "SEARCH_FOOD"
        else:
            self.state.current_goal = "EXPLORE"

        print("Goal:", self.state.current_goal)

    def act(self):

        if self.state.current_goal == "SEARCH_FOOD":
            self.client.command("Forward")
        elif self.state.current_goal == "EXPLORE":
            self.client.command("Forward")