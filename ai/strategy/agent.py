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

        if self.state.food() < 10:
            self.state.current_goal = "SEARCH_FOOD"
            return

        missing = self.state.missing_resources()
        if missing:
            self.state.current_goal = f"SEARCH_{missing[0].upper()}"
            return

        self.state.current_goal = "LEVEL_UP"

        print("Goal:", self.state.current_goal)

    def act(self):

        current_tile = self.state.current_tile()
        goal = self.state.current_goal

        if "food" in current_tile:
            print("Taking food")
            self.client.command("Take food")
            return
        
        if goal.startswith("SEARCH_"):
            ressource = goal.replace("SEARCH_", "").lower()
            self.search_resource(ressource)
            return

        if goal == "LEVEL_UP":
            self.level_up()
            return

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

    def move_to_tile(self, tile_index):
        if tile_index == 0:
            return

        if tile_index == 1:
            self.client.command("Left")
            self.client.command("Forward")
        elif tile_index == 2:
            self.client.command("Forward")
        elif tile_index == 3:
            self.client.command("Right")
            self.client.command("Forward")
        else:
            self.client.command("Forward")

    def level_up(self):

        if not self.state.ready_for_incantation():
            return

        requirements = self.state.requirements()
        for resource, amount in requirements.items():
            if resource == "players":
                continue

        for _ in range(amount):
            self.client.command(f"Set {resource}")

        response = self.client.command("Incantation")
        print("Incantation:", response)

