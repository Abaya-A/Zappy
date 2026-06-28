def tile_to_position(index):

    if index == 0:
        return 0, 0

    row = 1
    first_index = 1

    while index >= first_index + (2 * row + 1):
        first_index += 2 * row + 1
        row += 1

    offset = index - first_index
    x = offset - row
    y = row

    return x, y