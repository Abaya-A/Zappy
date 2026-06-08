#include "GuiProtocolHandlers.hpp"

#include "Resource.hpp"
#include "Tile.hpp"
#include "Player.hpp"

#include <iostream>

GuiProtocolHandlers::GuiProtocolHandlers(GameState &state)
    : _state(state),
      _handlers()
{
    registerHandlers();
}

bool GuiProtocolHandlers::expectArgCount(const ProtocolCommand &command, std::size_t expected) const
{
    if (command.hasArgCount(expected))
        return true;

    warnInvalid(command, "bad argument count");
    return false;
}

void GuiProtocolHandlers::warnInvalid(const ProtocolCommand &command, const std::string &reason) const
{
    std::cerr << "[WARN]: invalid " << command.name()
              << " command: " << reason
              << ": " << command.raw()
              << std::endl;
}

void GuiProtocolHandlers::warnRejected(const ProtocolCommand &command, const std::string &reason) const
{
    std::cerr << "[WARN]: rejected " << command.name()
              << " command: " << reason
              << ": " << command.raw()
              << std::endl;
}

std::optional<Player::Inventory> GuiProtocolHandlers::parsePlayerInventory(
    const ProtocolCommand &command,
    std::size_t startIndex
) const
{
    Player::Inventory inventory{};

    for (std::size_t i = 0; i < Resource::COUNT; ++i) {
        const auto quantity = command.intArg(startIndex + i);

        if (!quantity)
            return std::nullopt;

        inventory[i] = *quantity;
    }

    return inventory;
}

void GuiProtocolHandlers::registerHandlers()
{
    _handlers["msz"] = &GuiProtocolHandlers::handleMapSize;
    _handlers["bct"] = &GuiProtocolHandlers::handleTileContent;
    _handlers["tna"] = &GuiProtocolHandlers::handleTeamName;
    _handlers["sgt"] = &GuiProtocolHandlers::handleTimeUnit;
    _handlers["smg"] = &GuiProtocolHandlers::handleServerMessage;
    _handlers["seg"] = &GuiProtocolHandlers::handleEndGame;
    _handlers["pnw"] = &GuiProtocolHandlers::handlePlayerNew;
    _handlers["ppo"] = &GuiProtocolHandlers::handlePlayerPosition;
    _handlers["plv"] = &GuiProtocolHandlers::handlePlayerLevel;
    _handlers["pin"] = &GuiProtocolHandlers::handlePlayerInventory;
    _handlers["pdi"] = &GuiProtocolHandlers::handlePlayerDeath;
}

void GuiProtocolHandlers::handleCommand(const ProtocolCommand &command)
{
    auto handler = _handlers.find(command.name());

    if (handler == _handlers.end()) {
        std::cout << "command: " << command.name()
                  << " args=" << command.args().size() << std::endl;
        return;
    }

    (this->*(handler->second))(command);
}

void GuiProtocolHandlers::handleMapSize(const ProtocolCommand &command)
{
    if (!expectArgCount(command, 2))
        return;

    const auto width = command.intArg(0);
    const auto height = command.intArg(1);

    if (!width || !height) {
        warnInvalid(command, "bad map size values");
        return;
    }

    if (!_state.setMapSize(*width, *height)) {
        warnRejected(command, "map size update failed");
        return;
    }

    std::cout << "event: map size "
              << _state.width() << "x" << _state.height()
              << std::endl;
}

void GuiProtocolHandlers::handleTileContent(const ProtocolCommand &command)
{
    if (!command.hasArgCount(9)) {
        std::cerr << "[WARN]: invalid bct argument count: "
                  << command.raw() << std::endl;
        return;
    }

    const auto x = command.intArg(0);
    const auto y = command.intArg(1);

    if (!x || !y) {
        std::cerr << "[WARN]: invalid bct coordinates: "
                  << command.raw() << std::endl;
        return;
    }

    Tile tile;
    Tile::ResourceArray resources{};

    for (std::size_t i = 0; i < Resource::COUNT; ++i) {
        const auto quantity = command.intArg(i + 2);

        if (!quantity) {
            std::cerr << "[WARN]: invalid bct resource quantity: "
                      << command.raw() << std::endl;
            return;
        }

        resources[i] = *quantity;
    }

    if (!tile.setResources(resources)) {
        std::cerr << "[WARN]: rejected bct resources: "
                  << command.raw() << std::endl;
        return;
    }

    if (!_state.setTileContent(*x, *y, tile)) {
        std::cerr << "[WARN]: rejected bct tile position: "
                  << command.raw() << std::endl;
        return;
    }

    std::cout << "event: tile "
              << *x << "," << *y
              << " updated" << std::endl;
}

void GuiProtocolHandlers::handleTeamName(const ProtocolCommand &command)
{
    if (!expectArgCount(command, 1))
        return;

    const auto name = command.arg(0);

    if (!name) {
        warnInvalid(command, "bad team name value");
        return;
    }

    if (!_state.addTeam(*name)) {
        warnRejected(command, "team registration failed");
        return;
    }

    std::cout << "event: team " << *name << std::endl;
}

void GuiProtocolHandlers::handleTimeUnit(const ProtocolCommand &command)
{
    if (!expectArgCount(command, 1))
        return;

    const auto timeUnit = command.intArg(0);

    if (!timeUnit) {
        warnInvalid(command, "bad time unit value");
        return;
    }

    if (!_state.setTimeUnit(*timeUnit)) {
        warnRejected(command, "time unit update failed");
        return;
    }

    std::cout << "event: time unit "
              << _state.timeUnit()
              << std::endl;
}

void GuiProtocolHandlers::handleServerMessage(const ProtocolCommand &command)
{
    std::cout << "event: server message";

    for (const std::string &arg : command.args())
        std::cout << " " << arg;

    std::cout << std::endl;
}

void GuiProtocolHandlers::handleEndGame(const ProtocolCommand &command)
{
    if (!expectArgCount(command, 1))
        return;

    const auto team = command.arg(0);

    if (!team) {
        warnInvalid(command, "bad winner team value");
        return;
    }

    _state.setWinner(*team);

    std::cout << "event: end game winner="
              << *team
              << std::endl;
}

void GuiProtocolHandlers::handlePlayerNew(const ProtocolCommand &command)
{
    if (!expectArgCount(command, 6))
        return;

    const auto id = command.idArg(0);
    const auto x = command.intArg(1);
    const auto y = command.intArg(2);
    const auto orientation = command.intArg(3);
    const auto level = command.intArg(4);
    const auto teamName = command.arg(5);

    if (!id || !x || !y || !orientation || !level || !teamName) {
        warnInvalid(command, "bad new player values");
        return;
    }

    Player player(*id, *x, *y, *orientation, *level, *teamName);

    if (!_state.addPlayer(player)) {
        warnRejected(command, "player creation failed");
        return;
    }

    std::cout << "event: player #" << *id
              << " joined team " << *teamName
              << " at " << *x << "," << *y
              << " orientation=" << *orientation
              << " level=" << *level
              << std::endl;
}

void GuiProtocolHandlers::handlePlayerPosition(const ProtocolCommand &command)
{
    if (!expectArgCount(command, 4))
        return;

    const auto id = command.idArg(0);
    const auto x = command.intArg(1);
    const auto y = command.intArg(2);
    const auto orientation = command.intArg(3);

    if (!id || !x || !y || !orientation) {
        warnInvalid(command, "bad player position values");
        return;
    }

    if (!_state.updatePlayerPosition(*id, *x, *y, *orientation)) {
        warnRejected(command, "player position update failed");
        return;
    }

    std::cout << "event: player #" << *id
              << " moved to " << *x << "," << *y
              << " orientation=" << *orientation
              << std::endl;
}

void GuiProtocolHandlers::handlePlayerLevel(const ProtocolCommand &command)
{
    if (!expectArgCount(command, 2))
        return;

    const auto id = command.idArg(0);
    const auto level = command.intArg(1);

    if (!id || !level) {
        warnInvalid(command, "bad player level values");
        return;
    }

    if (!_state.updatePlayerLevel(*id, *level)) {
        warnRejected(command, "player level update failed");
        return;
    }

    std::cout << "event: player #" << *id
              << " level=" << *level
              << std::endl;
}

void GuiProtocolHandlers::handlePlayerInventory(const ProtocolCommand &command)
{
    if (!expectArgCount(command, 10))
        return;

    const auto id = command.idArg(0);
    const auto x = command.intArg(1);
    const auto y = command.intArg(2);
    const auto inventory = parsePlayerInventory(command, 3);

    if (!id || !x || !y || !inventory) {
        warnInvalid(command, "bad player inventory values");
        return;
    }

    if (!_state.updatePlayerInventory(*id, *x, *y, *inventory)) {
        warnRejected(command, "player inventory update failed");
        return;
    }

    std::cout << "event: player #" << *id
              << " inventory updated at " << *x << "," << *y
              << std::endl;
}

void GuiProtocolHandlers::handlePlayerDeath(const ProtocolCommand &command)
{
    if (!expectArgCount(command, 1))
        return;

    const auto id = command.idArg(0);

    if (!id) {
        warnInvalid(command, "bad player death value");
        return;
    }

    if (!_state.removePlayer(*id)) {
        warnRejected(command, "player removal failed");
        return;
    }

    std::cout << "event: player #" << *id
              << " died"
              << std::endl;
}