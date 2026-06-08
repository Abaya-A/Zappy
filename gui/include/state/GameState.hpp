#pragma once

#include "Egg.hpp"
#include "Player.hpp"
#include "Tile.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

struct Incantation {
    int x;
    int y;
    int level;
    std::vector<int> playerIds;
};

class GameState {
public:
    GameState();

    bool setMapSize(int width, int height);
    bool setTileContent(int x, int y, const Tile &tile);

    bool addTeam(const std::string &teamName);
    bool setTimeUnit(int timeUnit);
    void setWinner(const std::string &teamName);

    bool addPlayer(const Player &player);
    bool updatePlayerPosition(int id, int x, int y, int orientation);
    bool updatePlayerLevel(int id, int level);
    bool updatePlayerInventory(int id, int x, int y, const Player::Inventory &inventory);
    bool removePlayer(int id);

    bool addEgg(const Egg &egg);
    bool removeEgg(int id);

    int width() const;
    int height() const;
    int timeUnit() const;

    bool isReady() const;
    bool hasWinner() const;
    const std::optional<std::string> &winner() const;

    const Tile *tileAt(int x, int y) const;
    Tile *tileAt(int x, int y);

    const std::vector<Tile> &tiles() const;
    const std::vector<std::string> &teams() const;
    const std::unordered_map<int, Player> &players() const;
    const std::unordered_map<int, Egg> &eggs() const;

    bool startIncantation(int x, int y, int level, const std::vector<int> &playerIds);
    bool endIncantation(int x, int y);
    const std::vector<Incantation> &incantations() const;

private:
    int _width;
    int _height;
    int _timeUnit;
    std::optional<std::string> _winner;

    std::vector<Tile> _tiles;
    std::vector<std::string> _teams;
    std::unordered_map<int, Player> _players;
    std::unordered_map<int, Egg> _eggs;
    std::vector<Incantation> _incantations;

    bool isValidSize(int width, int height) const;
    bool isValidPosition(int x, int y) const;
    std::size_t tileIndex(int x, int y) const;
};