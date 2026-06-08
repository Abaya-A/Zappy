#pragma once

#include "GameState.hpp"
#include "GuiProtocolHandlers.hpp"
#include "NetworkClient.hpp"
#include "ProtocolParser.hpp"

#include <string>

class GuiClient {
public:
    GuiClient(const std::string &host, int port);

    bool connect();
    void run();

private:
    void handleLine(const std::string &line);
    void sendBootstrapRequests();

    std::string _host;
    int _port;
    NetworkClient _network;
    ProtocolParser _parser;
    GameState _state;
    GuiProtocolHandlers _protocolHandlers;
    bool _bootstrapSent;
};