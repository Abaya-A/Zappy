#include "protocol/GuiEventApplier.hpp"
#include "protocol/GuiProtocolEvent.hpp"
#include "state/GameState.hpp"

#include <iostream>

static void assertTrue(bool condition, const std::string &message)
{
    if (!condition) {
        std::cerr << "[FAIL] " << message << std::endl;
        std::exit(84);
    }
}

static void testApplyMapSize()
{
    GameState state;
    GuiEventApplier applier(state);

    const bool ok = applier.apply(MapSizeEvent{10, 12});

    assertTrue(ok, "MapSizeEvent should apply");
    assertTrue(state.width() == 10, "width should be 10");
    assertTrue(state.height() == 12, "height should be 12");
    assertTrue(state.isReady(), "state should be ready after map size");
}

static void testApplyPlayerLifecycle()
{
    GameState state;
    GuiEventApplier applier(state);

    assertTrue(applier.apply(MapSizeEvent{10, 10}), "map size should apply");

    assertTrue(applier.apply(PlayerNewEvent{1, 2, 3, 1, 1, "teamA"}), "player should be added");
    assertTrue(state.players().size() == 1, "state should contain one player");

    assertTrue(applier.apply(PlayerPositionEvent{1, 4, 5, 2}), "player position should update");
    assertTrue(state.players().at(1).x() == 4, "player x should be 4");
    assertTrue(state.players().at(1).y() == 5, "player y should be 5");
    assertTrue(state.players().at(1).orientation() == 2, "player orientation should be 2");

    assertTrue(applier.apply(PlayerDeathEvent{1}), "player should be removed");
    assertTrue(state.players().empty(), "state should contain no players");
}

static void testApplyEggLifecycle()
{
    GameState state;
    GuiEventApplier applier(state);

    assertTrue(applier.apply(MapSizeEvent{10, 10}), "map size should apply");

    assertTrue(applier.apply(EggNewEvent{3, 1, 4, 5}), "egg should be added");
    assertTrue(state.eggs().size() == 1, "state should contain one egg");

    assertTrue(applier.apply(EggHatchEvent{3}), "egg should hatch/remove");
    assertTrue(state.eggs().empty(), "state should contain no eggs");
}

int main()
{
    testApplyMapSize();
    testApplyPlayerLifecycle();
    testApplyEggLifecycle();

    std::cout << "[OK] event applier tests passed" << std::endl;
    return 0;
}