#include "Egg.hpp"

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

static void testEggCreation()
{
    Egg egg(12, 4, 7, 9);

    check(egg.id() == 12, "egg id is stored");
    check(egg.playerId() == 4, "parent player id is stored");
    check(egg.x() == 7, "egg x is stored");
    check(egg.y() == 9, "egg y is stored");
}

static void testInvalidConstructionFallback()
{
    Egg egg(-1, -2, -3, -4);

    check(egg.id() == 0, "invalid egg id falls back to 0");
    check(egg.playerId() == 0, "invalid parent player id falls back to 0");
    check(egg.x() == 0, "invalid x falls back to 0");
    check(egg.y() == 0, "invalid y falls back to 0");
}

static void testSetPosition()
{
    Egg egg(1, 2, 3, 4);

    check(egg.setPosition(10, 11), "valid egg position update succeeds");
    check(egg.x() == 10, "egg x is updated");
    check(egg.y() == 11, "egg y is updated");

    check(!egg.setPosition(-1, 5), "negative x position is rejected");
    check(egg.x() == 10, "invalid x does not mutate egg x");
    check(egg.y() == 11, "invalid x does not mutate egg y");

    check(!egg.setPosition(5, -1), "negative y position is rejected");
    check(egg.x() == 10, "invalid y does not mutate egg x");
    check(egg.y() == 11, "invalid y does not mutate egg y");
}

int main()
{
    testEggCreation();
    testInvalidConstructionFallback();
    testSetPosition();

    if (g_failed != 0)
        return 84;

    std::cout << "All egg tests passed" << std::endl;
    return 0;
}