#include "GuiClient.hpp"

#include "Resource.hpp"
#include "Tile.hpp"

#include <iostream>

void GuiClient::registerHandlers()
{
    _handlers["msz"] = &GuiClient::handleMapSize;
    _handlers["bct"] = &GuiClient::handleTileContent;
    _handlers["tna"] = &GuiClient::handleTeamName;
    _handlers["sgt"] = &GuiClient::handleTimeUnit;
    _handlers["smg"] = &GuiClient::handleServerMessage;
    _handlers["seg"] = &GuiClient::handleEndGame;
}

void GuiClient::handleCommand(const ProtocolCommand &command)
{
    auto handler = _handlers.find(command.name());

    if (handler == _handlers.end()) {
        std::cout << "command: " << command.name()
                  << " args=" << command.args().size() << std::endl;
        return;
    }

    (this->*(handler->second))(command);
}

void GuiClient::handleMapSize(const ProtocolCommand &command)
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

void GuiClient::handleTileContent(const ProtocolCommand &command)
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

void GuiClient::handleTeamName(const ProtocolCommand &command)
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

void GuiClient::handleTimeUnit(const ProtocolCommand &command)
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

void GuiClient::handleServerMessage(const ProtocolCommand &command)
{
    std::cout << "event: server message";

    for (const std::string &arg : command.args())
        std::cout << " " << arg;

    std::cout << std::endl;
}

void GuiClient::handleEndGame(const ProtocolCommand &command)
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