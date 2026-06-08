#pragma once

#include "GameState.hpp"
#include "ProtocolCommand.hpp"

#include <string>
#include <unordered_map>

class GuiProtocolHandlers {
public:
    explicit GuiProtocolHandlers(GameState &state);

    void handleCommand(const ProtocolCommand &command);

private:
    using Handler = void (GuiProtocolHandlers::*)(const ProtocolCommand &);

    void registerHandlers();

    void handleMapSize(const ProtocolCommand &command);
    void handleTileContent(const ProtocolCommand &command);
    void handleTeamName(const ProtocolCommand &command);
    void handleTimeUnit(const ProtocolCommand &command);
    void handleServerMessage(const ProtocolCommand &command);
    void handleEndGame(const ProtocolCommand &command);

    GameState &_state;
    std::unordered_map<std::string, Handler> _handlers;
};