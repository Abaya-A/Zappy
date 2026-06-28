def parse_inventory(text):
    inventory = {}

    text = text.strip()
    text = text.removeprefix("[")
    text = text.removesuffix("]")

    for item in text.split(","):
        parts = item.strip().split()

        if len(parts) != 2:
            continue
        name, count = parts

        if not count.isdigit():
            continue
        inventory[name] = int(count)

    return inventory

def parse_look(text):
    text = text.strip()
    text = text.removeprefix("[")
    text = text.removesuffix("]")

    tiles = []

    for tile in text.split(","):
        content = tile.strip()
        if content:
            tiles.append(content.split())
        else:
            tiles.append([])

    return tiles

def parse_broadcast(msg):
    parts = msg.split(",", 1)

    direction = int(parts[0].replace("message", "").strip())
    content = parts[1].strip()

    return direction, content
        