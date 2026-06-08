#include "GuiProtocolDecoder.hpp"
#include "ProtocolCommand.hpp"
#include "GuiProtocolEvent.hpp"

#include <iostream>
#include <variant>

static void assertTrue(bool condition, const std::string &message)
{
    if (!condition) {
        std::cerr << "[FAIL] " << message << std::endl;
        std::exit(84);
    }
}

static void testDecodeMapSize()
{
    GuiProtocolDecoder decoder;
    ProtocolCommand command("msz", {"10", "20"}, "msz 10 20");

    const auto event = decoder.decode(command);

    assertTrue(event.has_value(), "msz should decode");
    assertTrue(std::holds_alternative<MapSizeEvent>(*event), "msz should produce MapSizeEvent");

    const auto mapSize = std::get<MapSizeEvent>(*event);

    assertTrue(mapSize.width == 10, "msz width should be 10");
    assertTrue(mapSize.height == 20, "msz height should be 20");
}

static void testDecodePlayerNew()
{
    GuiProtocolDecoder decoder;
    ProtocolCommand command("pnw", {"#4", "2", "3", "1", "5", "teamA"}, "pnw #4 2 3 1 5 teamA");

    const auto event = decoder.decode(command);

    assertTrue(event.has_value(), "pnw should decode");
    assertTrue(std::holds_alternative<PlayerNewEvent>(*event), "pnw should produce PlayerNewEvent");

    const auto player = std::get<PlayerNewEvent>(*event);

    assertTrue(player.id == 4, "pnw id should be 4");
    assertTrue(player.x == 2, "pnw x should be 2");
    assertTrue(player.y == 3, "pnw y should be 3");
    assertTrue(player.orientation == 1, "pnw orientation should be 1");
    assertTrue(player.level == 5, "pnw level should be 5");
    assertTrue(player.teamName == "teamA", "pnw team should be teamA");
}

static void testDecodeEggNew()
{
    GuiProtocolDecoder decoder;
    ProtocolCommand command("enw", {"#8", "#4", "7", "9"}, "enw #8 #4 7 9");

    const auto event = decoder.decode(command);

    assertTrue(event.has_value(), "enw should decode");
    assertTrue(std::holds_alternative<EggNewEvent>(*event), "enw should produce EggNewEvent");

    const auto egg = std::get<EggNewEvent>(*event);

    assertTrue(egg.id == 8, "enw egg id should be 8");
    assertTrue(egg.playerId == 4, "enw player id should be 4");
    assertTrue(egg.x == 7, "enw x should be 7");
    assertTrue(egg.y == 9, "enw y should be 9");
}

static void testRejectInvalidCommand()
{
    GuiProtocolDecoder decoder;
    ProtocolCommand command("msz", {"10"}, "msz 10");

    const auto event = decoder.decode(command);

    assertTrue(!event.has_value(), "invalid msz should be rejected");
}

int main()
{
    testDecodeMapSize();
    testDecodePlayerNew();
    testDecodeEggNew();
    testRejectInvalidCommand();

    std::cout << "[OK] protocol decoder tests passed" << std::endl;
    return 0;
}