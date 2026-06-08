#pragma once

#include "Resource.hpp"

#include <array>
#include <string>

class Player {
public:
    using Inventory = std::array<int, Resource::COUNT>;

    Player(int id, int x, int y, int orientation, int level, std::string teamName);

    int id() const;
    int x() const;
    int y() const;
    int orientation() const;
    int level() const;
    const std::string &teamName() const;
    const Inventory &inventory() const;

    bool setPosition(int x, int y, int orientation);
    bool setLevel(int level);
    bool setInventory(int x, int y, const Inventory &inventory);

private:
    int _id;
    int _x;
    int _y;
    int _orientation;
    int _level;
    std::string _teamName;
    Inventory _inventory;

    static bool isValidId(int id);
    static bool isValidPosition(int x, int y);
    static bool isValidOrientation(int orientation);
    static bool isValidLevel(int level);
    static bool isValidInventory(const Inventory &inventory);
};