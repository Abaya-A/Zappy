#pragma once

#include <string>
#include <vector>

class NetworkClient {
public:
    NetworkClient();
    ~NetworkClient();

    NetworkClient(const NetworkClient &) = delete;
    NetworkClient &operator=(const NetworkClient &) = delete;

    bool connectToServer(const std::string &host, int port);
    void disconnect();

    bool isConnected() const;
    bool sendLine(const std::string &line);
    std::vector<std::string> pollLines();

private:
    int _fd;
    std::string _buffer;

    bool tryConnectAddress(struct addrinfo *address);
    bool setNonBlocking();
    bool receiveAvailableData();
    bool sendAll(const std::string &data);
    std::vector<std::string> extractLines();
};