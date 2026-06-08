#pragma once

#include "state/GameState.hpp"
#include "net/NetworkClient.hpp"
#include "protocol/ProtocolParser.hpp"
#include "protocol/GuiEventApplier.hpp"
#include "protocol/GuiProtocolDecoder.hpp"
#include "render/MagnumRenderer.hpp"

#include <string>

class GuiClient {
public:
    GuiClient(const std::string &host, int port, int argc, char **argv);

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
    GuiProtocolDecoder _decoder;
    GuiEventApplier _applier;
    MagnumRenderer _renderer;
    bool _bootstrapSent;
};