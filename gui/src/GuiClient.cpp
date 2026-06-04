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
    auto width = command.intArg(0);
    auto height = command.intArg(1);

    if (!command.hasArgCount(2) || !width.has_value() || !height.has_value()) {
        std::cerr << "[WARN]: invalid msz command: " << command.raw() << std::endl;
        return;
    }

    std::cout << "event: map size " << width.value() << "x" << height.value() << std::endl;
}

void GuiClient::handleTileContent(const ProtocolCommand &command)
{
    if (!command.hasArgCount(9)) {
        std::cerr << "[WARN]: invalid bct command: " << command.raw() << std::endl;
        return;
    }

    auto x = command.intArg(0);
    auto y = command.intArg(1);

    if (!x.has_value() || !y.has_value()) {
        std::cerr << "[WARN]: invalid bct coordinates: " << command.raw() << std::endl;
        return;
    }

    std::cout << "event: tile " << x.value() << "," << y.value() << " resources=[";

    for (std::size_t i = 2; i < 9; ++i) {
        auto resource = command.intArg(i);

        if (!resource.has_value()) {
            std::cerr << "\n[WARN]: invalid bct resource: " << command.raw() << std::endl;
            return;
        }

        if (i != 2)
            std::cout << ",";
        std::cout << resource.value();
    }

    std::cout << "]" << std::endl;
}

void GuiClient::handleTeamName(const ProtocolCommand &command)
{
    auto name = command.arg(0);

    if (!command.hasArgCount(1) || !name.has_value()) {
        std::cerr << "[WARN]: invalid tna command: " << command.raw() << std::endl;
        return;
    }

    std::cout << "event: team " << name.value() << std::endl;
}

void GuiClient::handleTimeUnit(const ProtocolCommand &command)
{
    auto timeUnit = command.intArg(0);

    if (!command.hasArgCount(1) || !timeUnit.has_value()) {
        std::cerr << "[WARN]: invalid sgt command: " << command.raw() << std::endl;
        return;
    }

    std::cout << "event: time unit " << timeUnit.value() << std::endl;
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
    auto team = command.arg(0);

    if (!command.hasArgCount(1) || !team.has_value()) {
        std::cerr << "[WARN]: invalid seg command: " << command.raw() << std::endl;
        return;
    }

    std::cout << "event: end game winner=" << team.value() << std::endl;
}

void GuiClient::sendBootstrapRequests()
{
    _network.sendLine("GRAPHIC");
    _network.sendLine("msz");
    _network.sendLine("mct");
    _network.sendLine("tna");
    _network.sendLine("sgt");
}