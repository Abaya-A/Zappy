#include "Egg.hpp"

Egg::Egg(int id, int playerId, int x, int y)
    : _id(isValidId(id) ? id : 0),
      _playerId(isValidId(playerId) ? playerId : 0),
      _x(isValidPosition(x, y) ? x : 0),
      _y(isValidPosition(x, y) ? y : 0)
{
}

int Egg::id() const
{
    return _id;
}

int Egg::playerId() const
{
    return _playerId;
}

int Egg::x() const
{
    return _x;
}

int Egg::y() const
{
    return _y;
}

bool Egg::setPosition(int x, int y)
{
    if (!isValidPosition(x, y))
        return false;

    _x = x;
    _y = y;
    return true;
}

bool Egg::isValidId(int id)
{
    return id >= 0;
}

bool Egg::isValidPosition(int x, int y)
{
    return x >= 0 && y >= 0;
}