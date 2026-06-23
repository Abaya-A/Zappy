def parse_inventory(text):
    result = {}

    text = text.strip()
    text = text.removeprefix("[")
    text = text.removesuffix("]")

    for part in text.split(","):
        tokens = part.strip().split()
        if len(tokens) == 2:
            result[tokens[0]] = int(tokens[1])

    return result

def parse_look(text):
    text = text.strip()
    text = text.removeprefix("[")
    text = text.removesuffix("]")

    tiles = []
    for tile in text.split(","):
        content = tile.strip()
        tiles.append(content.split() if content else [])

    return tiles