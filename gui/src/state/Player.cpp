#include "state/Player.hpp"

#include <utility>

Player::Player(int id, int x, int y, int orientation, int level, std::string teamName)
    : _id(isValidId(id) ? id : 0),
      _x(isValidPosition(x, y) ? x : 0),
      _y(isValidPosition(x, y) ? y : 0),
      _orientation(isValidOrientation(orientation) ? orientation : 1),
      _level(isValidLevel(level) ? level : 1),
      _teamName(std::move(teamName)),
      _inventory{}
{
}

int Player::id() const
{
    return _id;
}

int Player::x() const
{
    return _x;
}

int Player::y() const
{
    return _y;
}

int Player::orientation() const
{
    return _orientation;
}

int Player::level() const
{
    return _level;
}

const std::string &Player::teamName() const
{
    return _teamName;
}

const Player::Inventory &Player::inventory() const
{
    return _inventory;
}

bool Player::setPosition(int x, int y, int orientation)
{
    if (!isValidPosition(x, y) || !isValidOrientation(orientation))
        return false;

    _x = x;
    _y = y;
    _orientation = orientation;
    return true;
}

bool Player::setLevel(int level)
{
    if (!isValidLevel(level))
        return false;

    _level = level;
    return true;
}

bool Player::setInventory(int x, int y, const Inventory &inventory)
{
    if (!isValidPosition(x, y) || !isValidInventory(inventory))
        return false;

    _x = x;
    _y = y;
    _inventory = inventory;
    return true;
}

bool Player::isValidId(int id)
{
    return id >= 0;
}

bool Player::isValidPosition(int x, int y)
{
    return x >= 0 && y >= 0;
}

bool Player::isValidOrientation(int orientation)
{
    return orientation >= 1 && orientation <= 4;
}

bool Player::isValidLevel(int level)
{
    return level >= 1 && level <= 8;
}

bool Player::isValidInventory(const Inventory &inventory)
{
    for (int quantity : inventory) {
        if (quantity < 0)
            return false;
    }
    return true;
}