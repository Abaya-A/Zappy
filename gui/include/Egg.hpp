#pragma once

class Egg {
public:
    Egg(int id, int playerId, int x, int y);

    int id() const;
    int playerId() const;
    int x() const;
    int y() const;

    bool setPosition(int x, int y);

private:
    int _id;
    int _playerId;
    int _x;
    int _y;

    static bool isValidId(int id);
    static bool isValidPosition(int x, int y);
};