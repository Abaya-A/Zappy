from ai.strategy.levels import *

class State:
    def __init__(self):
        self.level = 1
        self.role = "WORKER"

        self.inventory = {}

        self.visible_tiles = []

        self.current_goal = None
        self.last_broadcast = None

        self.players_on_tile = 1
        self.messages = []
        self.is_leader = False
        self.waiting_for_players = False
        self.broadcast_cooldown = 0
        self.is_waiting_incantation = False

        self.fork_cooldown = 0

    def food(self):
        return self.inventory.get("food", 0)

    def current_tile(self):
        if not self.visible_tiles:
            return []
        return self.visible_tiles[0]

    def has_resource(self, resource, amount):
        return self.inventory.get(resource, 0) >= amount

    def find_resource(self, resource):
        for index, tile in enumerate(self.visible_tiles):
            if resource in tile:
                return index
        return None

    def requirements(self):
        return LEVEL_REQUIREMENTS[self.level]

    def missing_resources(self):
        missing = []

        requirements = self.requirements()

        for resource, amount in requirements.items():
            if resource == "players":
                continue

            current = self.inventory.get(resource, 0)

            if current < amount:
                missing.append(resource)

        return missing

    def ready_for_incantation(self):
        req = self.requirements()
        required_players = req.get("players", 1)

        if self.player_count_on_tile() != required_players:
            return False

        for resource, amount in req.items():
            if resource == "players":
                continue
            if self.inventory.get(resource, 0) < amount:
                return False

        return True
    
    def next_missing_resource(self):
        missing = self.missing_resources()

        for resource in RESOURCE_PRIORITY:
            if resource in missing:
                return resource

        return None
    
    def player_count_on_tile(self):
        return self.current_tile().count("player")