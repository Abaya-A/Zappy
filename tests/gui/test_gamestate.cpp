#include "state/GameState.hpp"

#include <iostream>

static int g_failed = 0;

static void check(bool condition, const char *message)
{
    if (!condition) {
        std::cerr << "[FAIL] " << message << std::endl;
        ++g_failed;
        return;
    }
    std::cout << "[ OK ] " << message << std::endl;
}

static void testMapSize()
{
    GameState state;

    check(!state.setMapSize(0, 5), "invalid width is rejected");
    check(!state.setMapSize(5, 0), "invalid height is rejected");
    check(state.setMapSize(10, 8), "valid map size is accepted");
    check(state.width() == 10, "width is stored");
    check(state.height() == 8, "height is stored");
    check(state.tileAt(0, 0) != nullptr, "tile 0,0 exists");
    check(state.tileAt(9, 7) != nullptr, "last tile exists");
    check(state.tileAt(10, 7) == nullptr, "out of bounds x is rejected");
    check(state.tileAt(9, 8) == nullptr, "out of bounds y is rejected");
}

static void testTileContent()
{
    GameState state;
    Tile tile;

    check(state.setMapSize(4, 4), "map size set before tile update");
    check(tile.setQuantity(ResourceType::Food, 3), "test tile food set");
    check(state.setTileContent(2, 1, tile), "valid tile content update succeeds");

    const Tile *stored = state.tileAt(2, 1);

    check(stored != nullptr, "stored tile is accessible");
    check(stored->quantity(ResourceType::Food) == 3, "stored tile content is correct");
    check(!state.setTileContent(4, 1, tile), "invalid tile x is rejected");
    check(!state.setTileContent(1, 4, tile), "invalid tile y is rejected");
}

static void testTeams()
{
    GameState state;

    check(state.addTeam("TeamA"), "team A is added");
    check(state.addTeam("TeamB"), "team B is added");
    check(!state.addTeam("TeamA"), "duplicate team is rejected");
    check(!state.addTeam(""), "empty team name is rejected");
    check(state.teams().size() == 2, "team count is correct");
}

static void testTimeUnitAndWinner()
{
    GameState state;

    check(!state.setTimeUnit(0), "zero time unit is rejected");
    check(!state.setTimeUnit(-1), "negative time unit is rejected");
    check(state.setTimeUnit(100), "valid time unit is accepted");
    check(state.timeUnit() == 100, "time unit is stored");

    check(!state.hasWinner(), "winner is initially empty");
    state.setWinner("TeamA");
    check(state.hasWinner(), "winner is set");
    check(state.winner().value_or("") == "TeamA", "winner value is correct");
}

static void testPlayers()
{
    GameState state;
    Player player(42, 1, 2, 3, 4, "TeamA");
    Player::Inventory inventory = {10, 1, 2, 3, 4, 5, 6};

    check(state.addPlayer(player), "player is added");
    check(!state.addPlayer(player), "duplicate player is rejected");
    check(state.players().size() == 1, "player count is correct");

    check(state.updatePlayerPosition(42, 5, 6, 2), "player position update succeeds");
    check(state.players().at(42).x() == 5, "player x is updated");
    check(state.players().at(42).orientation() == 2, "player orientation is updated");

    check(state.updatePlayerLevel(42, 8), "player level update succeeds");
    check(state.players().at(42).level() == 8, "player level is updated");

    check(state.updatePlayerInventory(42, 7, 8, inventory), "player inventory update succeeds");
    check(state.players().at(42).inventory()[Resource::index(ResourceType::Food)] == 10,
        "player inventory is updated");

    check(!state.updatePlayerPosition(404, 1, 1, 1), "missing player position update fails");
    check(state.removePlayer(42), "player is removed");
    check(!state.removePlayer(42), "missing player removal fails");
}

static void testEggs()
{
    GameState state;
    Egg egg(12, 42, 3, 4);

    check(state.addEgg(egg), "egg is added");
    check(!state.addEgg(egg), "duplicate egg is rejected");
    check(state.eggs().size() == 1, "egg count is correct");

    check(state.removeEgg(12), "egg is removed");
    check(!state.removeEgg(12), "missing egg removal fails");
}

int main()
{
    testMapSize();
    testTileContent();
    testTeams();
    testTimeUnitAndWinner();
    testPlayers();
    testEggs();

    if (g_failed != 0)
        return 84;

    std::cout << "All game state tests passed" << std::endl;
    return 0;
}