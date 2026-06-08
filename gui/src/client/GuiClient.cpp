#include "client/GuiClient.hpp"

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
      _decoder(),
      _applier(_state),
      _bootstrapSent(false)
{
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

    const auto event = _decoder.decode(command.value());

    if (!event.has_value()) {
        std::cerr << "[WARN]: invalid or unsupported protocol command: "
                  << command->raw() << std::endl;
        return;
    }

    if (!_applier.apply(event.value())) {
        std::cerr << "[WARN]: rejected protocol event: "
                  << command->raw() << std::endl;
        return;
    }
}

void GuiClient::sendBootstrapRequests()
{
    _network.sendLine("GRAPHIC");
    _network.sendLine("msz");
    _network.sendLine("mct");
    _network.sendLine("tna");
    _network.sendLine("sgt");
}