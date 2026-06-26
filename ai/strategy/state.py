from ai.strategy.levels import LEVEL_REQUIREMENTS

class State:
    def __init__(self):

        self.level = 1
        self.inventory = {}
        self.visible_tiles = []
        self.current_goal = None
        self.last_broadcast = None

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
        return len(self.missing_resources()) == 0