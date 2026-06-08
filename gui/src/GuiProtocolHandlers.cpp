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
    if (!command.hasArgCount(2)) {
        std::cerr << "[WARN]: invalid msz argument count: "
                  << command.raw() << std::endl;
        return;
    }

    const auto width = command.intArg(0);
    const auto height = command.intArg(1);

    if (!width || !height) {
        std::cerr << "[WARN]: invalid msz values: "
                  << command.raw() << std::endl;
        return;
    }

    if (!_state.setMapSize(*width, *height)) {
        std::cerr << "[WARN]: rejected map size: "
                  << command.raw() << std::endl;
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
    if (!command.hasArgCount(1)) {
        std::cerr << "[WARN]: invalid tna argument count: "
                  << command.raw() << std::endl;
        return;
    }

    const auto name = command.arg(0);

    if (!name) {
        std::cerr << "[WARN]: invalid tna value: "
                  << command.raw() << std::endl;
        return;
    }

    if (!_state.addTeam(*name)) {
        std::cerr << "[WARN]: rejected team name: "
                  << command.raw() << std::endl;
        return;
    }

    std::cout << "event: team " << *name << std::endl;
}

void GuiProtocolHandlers::handleTimeUnit(const ProtocolCommand &command)
{
    if (!command.hasArgCount(1)) {
        std::cerr << "[WARN]: invalid sgt argument count: "
                  << command.raw() << std::endl;
        return;
    }

    const auto timeUnit = command.intArg(0);

    if (!timeUnit) {
        std::cerr << "[WARN]: invalid sgt value: "
                  << command.raw() << std::endl;
        return;
    }

    if (!_state.setTimeUnit(*timeUnit)) {
        std::cerr << "[WARN]: rejected time unit: "
                  << command.raw() << std::endl;
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
    if (!command.hasArgCount(1)) {
        std::cerr << "[WARN]: invalid seg argument count: "
                  << command.raw() << std::endl;
        return;
    }

    const auto team = command.arg(0);

    if (!team) {
        std::cerr << "[WARN]: invalid seg value: "
                  << command.raw() << std::endl;
        return;
    }

    _state.setWinner(*team);

    std::cout << "event: end game winner="
              << *team
              << std::endl;
}

void GuiProtocolHandlers::handlePlayerNew(const ProtocolCommand &command)
{
    if (!command.hasArgCount(6)) {
        std::cerr << "[WARN]: invalid pnw argument count: "
                  << command.raw() << std::endl;
        return;
    }

    const auto id = command.idArg(0);
    const auto x = command.intArg(1);
    const auto y = command.intArg(2);
    const auto orientation = command.intArg(3);
    const auto level = command.intArg(4);
    const auto teamName = command.arg(5);

    if (!id || !x || !y || !orientation || !level || !teamName) {
        std::cerr << "[WARN]: invalid pnw values: "
                  << command.raw() << std::endl;
        return;
    }

    Player player(*id, *x, *y, *orientation, *level, *teamName);

    if (!_state.addPlayer(player)) {
        std::cerr << "[WARN]: rejected new player: "
                  << command.raw() << std::endl;
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
    if (!command.hasArgCount(4)) {
        std::cerr << "[WARN]: invalid ppo argument count: "
                  << command.raw() << std::endl;
        return;
    }

    const auto id = command.idArg(0);
    const auto x = command.intArg(1);
    const auto y = command.intArg(2);
    const auto orientation = command.intArg(3);

    if (!id || !x || !y || !orientation) {
        std::cerr << "[WARN]: invalid ppo values: "
                  << command.raw() << std::endl;
        return;
    }

    if (!_state.updatePlayerPosition(*id, *x, *y, *orientation)) {
        std::cerr << "[WARN]: rejected player position: "
                  << command.raw() << std::endl;
        return;
    }

    std::cout << "event: player #" << *id
              << " moved to " << *x << "," << *y
              << " orientation=" << *orientation
              << std::endl;
}

void GuiProtocolHandlers::handlePlayerLevel(const ProtocolCommand &command)
{
    if (!command.hasArgCount(2)) {
        std::cerr << "[WARN]: invalid plv argument count: "
                  << command.raw() << std::endl;
        return;
    }

    const auto id = command.idArg(0);
    const auto level = command.intArg(1);

    if (!id || !level) {
        std::cerr << "[WARN]: invalid plv values: "
                  << command.raw() << std::endl;
        return;
    }

    if (!_state.updatePlayerLevel(*id, *level)) {
        std::cerr << "[WARN]: rejected player level: "
                  << command.raw() << std::endl;
        return;
    }

    std::cout << "event: player #" << *id
              << " level=" << *level
              << std::endl;
}

void GuiProtocolHandlers::handlePlayerInventory(const ProtocolCommand &command)
{
    if (!command.hasArgCount(10)) {
        std::cerr << "[WARN]: invalid pin argument count: "
                  << command.raw() << std::endl;
        return;
    }

    const auto id = command.idArg(0);
    const auto x = command.intArg(1);
    const auto y = command.intArg(2);

    if (!id || !x || !y) {
        std::cerr << "[WARN]: invalid pin position values: "
                  << command.raw() << std::endl;
        return;
    }

    Player::Inventory inventory{};

    for (std::size_t i = 0; i < Resource::COUNT; ++i) {
        const auto quantity = command.intArg(i + 3);

        if (!quantity) {
            std::cerr << "[WARN]: invalid pin inventory value: "
                      << command.raw() << std::endl;
            return;
        }

        inventory[i] = *quantity;
    }

    if (!_state.updatePlayerInventory(*id, *x, *y, inventory)) {
        std::cerr << "[WARN]: rejected player inventory: "
                  << command.raw() << std::endl;
        return;
    }

    std::cout << "event: player #" << *id
              << " inventory updated at " << *x << "," << *y
              << std::endl;
}

void GuiProtocolHandlers::handlePlayerDeath(const ProtocolCommand &command)
{
    if (!command.hasArgCount(1)) {
        std::cerr << "[WARN]: invalid pdi argument count: "
                  << command.raw() << std::endl;
        return;
    }

    const auto id = command.idArg(0);

    if (!id) {
        std::cerr << "[WARN]: invalid pdi value: "
                  << command.raw() << std::endl;
        return;
    }

    if (!_state.removePlayer(*id)) {
        std::cerr << "[WARN]: rejected player death: "
                  << command.raw() << std::endl;
        return;
    }

    std::cout << "event: player #" << *id
              << " died"
              << std::endl;
}