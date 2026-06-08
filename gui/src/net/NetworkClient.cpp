#include "net/NetworkClient.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

NetworkClient::NetworkClient()
    : _fd(-1), _buffer()
{
}

NetworkClient::~NetworkClient()
{
    disconnect();
}

bool NetworkClient::connectToServer(const std::string &host, int port)
{
    struct addrinfo hints {};
    struct addrinfo *addresses = nullptr;
    const std::string service = std::to_string(port);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host.c_str(), service.c_str(), &hints, &addresses) != 0) {
        std::cerr << "[ERROR]: cannot resolve host: " << host << std::endl;
        return false;
    }

    for (struct addrinfo *current = addresses; current != nullptr; current = current->ai_next) {
        if (tryConnectAddress(current))
            break;
    }

    freeaddrinfo(addresses);

    if (_fd == -1) {
        std::cerr << "[ERROR]: cannot connect to " << host << ":" << port << std::endl;
        return false;
    }

    if (!setNonBlocking()) {
        disconnect();
        return false;
    }

    return true;
}

bool NetworkClient::tryConnectAddress(struct addrinfo *address)
{
    int socketFd = socket(address->ai_family, address->ai_socktype, address->ai_protocol);

    if (socketFd == -1)
        return false;

    if (connect(socketFd, address->ai_addr, address->ai_addrlen) == -1) {
        close(socketFd);
        return false;
    }

    _fd = socketFd;
    return true;
}

void NetworkClient::disconnect()
{
    if (_fd != -1) {
        close(_fd);
        _fd = -1;
    }
    _buffer.clear();
}

bool NetworkClient::isConnected() const
{
    return _fd != -1;
}

bool NetworkClient::sendLine(const std::string &line)
{
    std::string data = line;

    if (!isConnected())
        return false;
    if (data.empty() || data.back() != '\n')
        data += '\n';
    return sendAll(data);
}

bool NetworkClient::sendAll(const std::string &data)
{
    const char *cursor = data.c_str();
    std::size_t remaining = data.size();

    while (remaining > 0) {
        ssize_t sent = send(_fd, cursor, remaining, 0);

        if (sent > 0) {
            cursor += sent;
            remaining -= static_cast<std::size_t>(sent);
            continue;
        }

        if (sent == -1 && errno == EINTR)
            continue;

        if (sent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
            continue;

        std::cerr << "[ERROR]: send failed: " << std::strerror(errno) << std::endl;
        disconnect();
        return false;
    }

    return true;
}

std::vector<std::string> NetworkClient::pollLines()
{
    if (!isConnected())
        return {};

    receiveAvailableData();
    return extractLines();
}

bool NetworkClient::receiveAvailableData()
{
    char chunk[4096];

    while (true) {
        ssize_t received = recv(_fd, chunk, sizeof(chunk), 0);

        if (received > 0) {
            _buffer.append(chunk, static_cast<std::size_t>(received));
            continue;
        }

        if (received == 0) {
            std::cerr << "[INFO]: server closed connection" << std::endl;
            disconnect();
            return false;
        }

        if (errno == EINTR)
            continue;

        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return true;

        std::cerr << "[ERROR]: recv failed: " << std::strerror(errno) << std::endl;
        disconnect();
        return false;
    }
}

std::vector<std::string> NetworkClient::extractLines()
{
    std::vector<std::string> lines;
    std::size_t newlinePos = _buffer.find('\n');

    while (newlinePos != std::string::npos) {
        std::string line = _buffer.substr(0, newlinePos);

        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        lines.push_back(line);
        _buffer.erase(0, newlinePos + 1);
        newlinePos = _buffer.find('\n');
    }

    return lines;
}

bool NetworkClient::setNonBlocking()
{
    int flags = fcntl(_fd, F_GETFL, 0);

    if (flags == -1) {
        std::cerr << "[ERROR]: fcntl failed: " << std::strerror(errno) << std::endl;
        return false;
    }

    if (fcntl(_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "[ERROR]: fcntl failed: " << std::strerror(errno) << std::endl;
        return false;
    }

    return true;
}