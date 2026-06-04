#pragma once

#include "NetworkClient.hpp"
#include "ProtocolCommand.hpp"
#include "ProtocolParser.hpp"

#include <string>
#include <unordered_map>

class GuiClient {
public:
    GuiClient(const std::string &host, int port);

    bool connect();
    void run();

private:
    using CommandHandler = void (GuiClient::*)(const ProtocolCommand &);

    std::string _host;
    int _port;
    NetworkClient _network;
    ProtocolParser _parser;
    std::unordered_map<std::string, CommandHandler> _handlers;
    bool _bootstrapSent;

    void handleLine(const std::string &line);
    void sendBootstrapRequests();

    void registerHandlers();
    void handleCommand(const ProtocolCommand &command);

    void handleMapSize(const ProtocolCommand &command);
    void handleTileContent(const ProtocolCommand &command);
    void handleTeamName(const ProtocolCommand &command);
    void handleTimeUnit(const ProtocolCommand &command);
    void handleServerMessage(const ProtocolCommand &command);
    void handleEndGame(const ProtocolCommand &command);
};