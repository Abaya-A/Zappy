#include "state/Incantation.hpp"

#include <utility>

Incantation::Incantation(int x, int y, int level, std::vector<int> playerIds)
    : _x(x),
      _y(y),
      _level(level),
      _playerIds(std::move(playerIds))
{
}

int Incantation::x() const
{
    return _x;
}

int Incantation::y() const
{
    return _y;
}

int Incantation::level() const
{
    return _level;
}

const std::vector<int> &Incantation::playerIds() const
{
    return _playerIds;
}

bool Incantation::isAt(int x, int y) const
{
    return _x == x && _y == y;
}