#pragma once

#include <vector>

class Incantation {
public:
    Incantation(int x, int y, int level, std::vector<int> playerIds);

    int x() const;
    int y() const;
    int level() const;
    const std::vector<int> &playerIds() const;

    bool isAt(int x, int y) const;

private:
    int _x;
    int _y;
    int _level;
    std::vector<int> _playerIds;
};