#include "state/Broadcast.hpp"

#include <utility>

Broadcast::Broadcast(int playerId, int x, int y, std::string message)
    : _playerId(playerId),
      _x(x),
      _y(y),
      _message(std::move(message)),
      _remainingTicks(DefaultLifetimeTicks)
{
}

int Broadcast::playerId() const
{
    return _playerId;
}

int Broadcast::x() const
{
    return _x;
}

int Broadcast::y() const
{
    return _y;
}

const std::string &Broadcast::message() const
{
    return _message;
}

bool Broadcast::belongsToPlayer(int playerId) const
{
    return _playerId == playerId;
}

void Broadcast::tick()
{
    if (_remainingTicks > 0)
        --_remainingTicks;
}

bool Broadcast::isExpired() const
{
    return _remainingTicks <= 0;
}