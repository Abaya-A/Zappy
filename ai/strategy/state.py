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