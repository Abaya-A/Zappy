#include "GameState.hpp"

#include <iostream>

static void assertTrue(bool condition, const std::string &message)
{
    if (!condition) {
        std::cerr << "[FAIL] " << message << std::endl;
        std::exit(84);
    }
}

int main()
{
    GameState state;

    assertTrue(!state.isReady(), "state should not be ready before map size");

    assertTrue(state.setMapSize(4, 3), "setMapSize should succeed");
    assertTrue(state.isReady(), "state should be ready after map size");
    assertTrue(state.tiles().size() == 12, "tiles size should be width * height");

    const Tile *tile = state.tileAt(2, 1);

    assertTrue(tile != nullptr, "tileAt valid position should return tile");
    assertTrue(state.tileAt(9, 9) == nullptr, "tileAt invalid position should return nullptr");

    std::cout << "[OK] gamestate read api tests passed" << std::endl;
    return 0;
}