#pragma once

#include <string>

class Broadcast {
public:
    static constexpr int DefaultLifetimeTicks = 180;

    Broadcast(int playerId, int x, int y, std::string message);

    int playerId() const;
    int x() const;
    int y() const;
    const std::string &message() const;

    bool belongsToPlayer(int playerId) const;
    void tick();
    bool isExpired() const;

private:
    int _playerId;
    int _x;
    int _y;
    std::string _message;
    int _remainingTicks;
};