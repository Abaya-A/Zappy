#include "state/Player.hpp"

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

static void testPlayerCreation()
{
    Player player(42, 5, 6, 2, 3, "TeamA");

    check(player.id() == 42, "player id is stored");
    check(player.x() == 5, "player x is stored");
    check(player.y() == 6, "player y is stored");
    check(player.orientation() == 2, "player orientation is stored");
    check(player.level() == 3, "player level is stored");
    check(player.teamName() == "TeamA", "player team name is stored");
}

static void testInvalidConstructionFallback()
{
    Player player(-1, -5, -6, 9, 42, "TeamA");

    check(player.id() == 0, "invalid player id falls back to 0");
    check(player.x() == 0, "invalid player x falls back to 0");
    check(player.y() == 0, "invalid player y falls back to 0");
    check(player.orientation() == 1, "invalid orientation falls back to 1");
    check(player.level() == 1, "invalid level falls back to 1");
}

static void testSetPosition()
{
    Player player(1, 2, 3, 1, 1, "TeamA");

    check(player.setPosition(10, 11, 4), "valid position update succeeds");
    check(player.x() == 10, "player x is updated");
    check(player.y() == 11, "player y is updated");
    check(player.orientation() == 4, "player orientation is updated");

    check(!player.setPosition(-1, 11, 4), "negative x is rejected");
    check(player.x() == 10, "invalid position does not mutate x");

    check(!player.setPosition(10, -1, 4), "negative y is rejected");
    check(player.y() == 11, "invalid position does not mutate y");

    check(!player.setPosition(10, 11, 0), "invalid orientation 0 is rejected");
    check(player.orientation() == 4, "invalid orientation does not mutate");

    check(!player.setPosition(10, 11, 5), "invalid orientation 5 is rejected");
    check(player.orientation() == 4, "invalid orientation upper bound does not mutate");
}

static void testSetLevel()
{
    Player player(1, 2, 3, 1, 1, "TeamA");

    check(player.setLevel(8), "valid level update succeeds");
    check(player.level() == 8, "player level is updated");

    check(!player.setLevel(0), "level 0 is rejected");
    check(player.level() == 8, "invalid low level does not mutate");

    check(!player.setLevel(9), "level 9 is rejected");
    check(player.level() == 8, "invalid high level does not mutate");
}

static void testSetInventory()
{
    Player player(1, 2, 3, 1, 1, "TeamA");
    Player::Inventory inventory = {10, 1, 2, 3, 4, 5, 6};

    check(player.setInventory(7, 8, inventory), "valid inventory update succeeds");
    check(player.x() == 7, "inventory update sets x");
    check(player.y() == 8, "inventory update sets y");
    check(player.inventory()[Resource::index(ResourceType::Food)] == 10, "food inventory is updated");
    check(player.inventory()[Resource::index(ResourceType::Thystame)] == 6, "thystame inventory is updated");
}

static void testRejectInvalidInventory()
{
    Player player(1, 2, 3, 1, 1, "TeamA");
    Player::Inventory valid = {10, 1, 2, 3, 4, 5, 6};
    Player::Inventory invalid = {10, 1, -2, 3, 4, 5, 6};

    check(player.setInventory(7, 8, valid), "valid inventory is accepted");
    check(!player.setInventory(9, 10, invalid), "negative inventory quantity is rejected");
    check(player.x() == 7, "invalid inventory does not mutate x");
    check(player.y() == 8, "invalid inventory does not mutate y");
    check(player.inventory()[Resource::index(ResourceType::Deraumere)] == 2,
        "invalid inventory does not mutate resources");
}

int main()
{
    testPlayerCreation();
    testInvalidConstructionFallback();
    testSetPosition();
    testSetLevel();
    testSetInventory();
    testRejectInvalidInventory();

    if (g_failed != 0)
        return 84;

    std::cout << "All player tests passed" << std::endl;
    return 0;
}