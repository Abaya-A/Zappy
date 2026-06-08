#include "GuiClient.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

GuiClient::GuiClient(const std::string &host, int port)
    : _host(host),
      _port(port),
      _network(),
      _parser(),
      _state(),
      _handlers(),
      _bootstrapSent(false)
{
    registerHandlers();
}

bool GuiClient::connect()
{
    return _network.connectToServer(_host, _port);
}

void GuiClient::run()
{
    std::cout << "connected to " << _host << ":" << _port << std::endl;

    while (_network.isConnected()) {
        std::vector<std::string> lines = _network.pollLines();

        for (const std::string &line : lines)
            handleLine(line);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void GuiClient::handleLine(const std::string &line)
{
    std::cout << "server: " << line << std::endl;

    if (line == "WELCOME" && !_bootstrapSent) {
        sendBootstrapRequests();
        _bootstrapSent = true;
        std::cout << "sent GRAPHIC bootstrap requests" << std::endl;
        return;
    }

    std::optional<ProtocolCommand> command = _parser.parseLine(line);

    if (!command.has_value()) {
        std::cerr << "[WARN]: invalid protocol line: " << line << std::endl;
        return;
    }

    handleCommand(command.value());
}

void GuiClient::sendBootstrapRequests()
{
    _network.sendLine("GRAPHIC");
    _network.sendLine("msz");
    _network.sendLine("mct");
    _network.sendLine("tna");
    _network.sendLine("sgt");
}