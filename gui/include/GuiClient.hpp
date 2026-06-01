#pragma once

#include "NetworkClient.hpp"

#include <string>

class GuiClient {
public:
    GuiClient(const std::string &host, int port);

    bool connect();
    void run();

private:
    std::string _host;
    int _port;
    NetworkClient _network;
    bool _bootstrapSent;

    void handleLine(const std::string &line);
    void sendBootstrapRequests();
};