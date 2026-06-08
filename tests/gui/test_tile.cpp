#include "state/Tile.hpp"

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

static void testDefaultTile()
{
    Tile tile;

    check(tile.isEmpty(), "default tile is empty");
    check(tile.totalResources() == 0, "default total resources is 0");
    check(tile.quantity(ResourceType::Food) == 0, "default food quantity is 0");
    check(tile.quantity(ResourceType::Thystame) == 0, "default thystame quantity is 0");
}

static void testSetQuantity()
{
    Tile tile;

    check(tile.setQuantity(ResourceType::Food, 5), "set food quantity succeeds");
    check(tile.quantity(ResourceType::Food) == 5, "food quantity is updated");
    check(tile.totalResources() == 5, "total resources updates after set quantity");

    check(!tile.setQuantity(ResourceType::Food, -1), "negative quantity is rejected");
    check(tile.quantity(ResourceType::Food) == 5, "invalid quantity does not mutate tile");
}

static void testSetResources()
{
    Tile tile;
    Tile::ResourceArray resources = {1, 2, 3, 4, 5, 6, 7};

    check(tile.setResources(resources), "set resources succeeds");
    check(tile.quantity(ResourceType::Food) == 1, "food quantity from array is correct");
    check(tile.quantity(ResourceType::Linemate) == 2, "linemate quantity from array is correct");
    check(tile.quantity(ResourceType::Thystame) == 7, "thystame quantity from array is correct");
    check(tile.totalResources() == 28, "total resources from array is correct");
}

static void testRejectInvalidResources()
{
    Tile tile;
    Tile::ResourceArray valid = {1, 2, 3, 4, 5, 6, 7};
    Tile::ResourceArray invalid = {1, 2, -3, 4, 5, 6, 7};

    check(tile.setResources(valid), "valid resources are accepted");
    check(!tile.setResources(invalid), "negative resource array is rejected");
    check(tile.quantity(ResourceType::Deraumere) == 3, "invalid resource array does not mutate tile");
}

int main()
{
    testDefaultTile();
    testSetQuantity();
    testSetResources();
    testRejectInvalidResources();

    if (g_failed != 0)
        return 84;

    std::cout << "All tile tests passed" << std::endl;
    return 0;
}