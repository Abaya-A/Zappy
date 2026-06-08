#include "state/GameState.hpp"

#include <algorithm>

GameState::GameState()
    : _width(0),
      _height(0),
      _timeUnit(0),
      _winner(std::nullopt),
      _tiles(),
      _teams(),
      _players(),
      _eggs(),
      _incantations()
{
}

bool GameState::setMapSize(int width, int height)
{
    if (!isValidSize(width, height))
        return false;

    _width = width;
    _height = height;
    _tiles.clear();
    _tiles.resize(static_cast<std::size_t>(_width * _height));
    return true;
}

bool GameState::setTileContent(int x, int y, const Tile &tile)
{
    if (!isValidPosition(x, y))
        return false;

    _tiles[tileIndex(x, y)] = tile;
    return true;
}

bool GameState::addTeam(const std::string &teamName)
{
    if (teamName.empty())
        return false;

    if (std::find(_teams.begin(), _teams.end(), teamName) != _teams.end())
        return false;

    _teams.push_back(teamName);
    return true;
}

bool GameState::setTimeUnit(int timeUnit)
{
    if (timeUnit <= 0)
        return false;

    _timeUnit = timeUnit;
    return true;
}

void GameState::setWinner(const std::string &teamName)
{
    _winner = teamName;
}

bool GameState::addPlayer(const Player &player)
{
    if (_players.find(player.id()) != _players.end())
        return false;

    _players.emplace(player.id(), player);
    return true;
}

bool GameState::updatePlayerPosition(int id, int x, int y, int orientation)
{
    auto it = _players.find(id);

    if (it == _players.end())
        return false;

    return it->second.setPosition(x, y, orientation);
}

bool GameState::updatePlayerLevel(int id, int level)
{
    auto it = _players.find(id);

    if (it == _players.end())
        return false;

    return it->second.setLevel(level);
}

bool GameState::updatePlayerInventory(int id, int x, int y, const Player::Inventory &inventory)
{
    auto it = _players.find(id);

    if (it == _players.end())
        return false;

    return it->second.setInventory(x, y, inventory);
}

bool GameState::removePlayer(int id)
{
    return _players.erase(id) > 0;
}

bool GameState::addEgg(const Egg &egg)
{
    if (_eggs.find(egg.id()) != _eggs.end())
        return false;

    _eggs.emplace(egg.id(), egg);
    return true;
}

bool GameState::removeEgg(int id)
{
    return _eggs.erase(id) > 0;
}

int GameState::width() const
{
    return _width;
}

int GameState::height() const
{
    return _height;
}

int GameState::timeUnit() const
{
    return _timeUnit;
}

bool GameState::hasWinner() const
{
    return _winner.has_value();
}

const std::optional<std::string> &GameState::winner() const
{
    return _winner;
}

const Tile *GameState::tileAt(int x, int y) const
{
    if (!isValidPosition(x, y))
        return nullptr;

    return &_tiles[tileIndex(x, y)];
}

Tile *GameState::tileAt(int x, int y)
{
    if (!isValidPosition(x, y))
        return nullptr;

    return &_tiles[tileIndex(x, y)];
}

const std::vector<std::string> &GameState::teams() const
{
    return _teams;
}

const std::unordered_map<int, Player> &GameState::players() const
{
    return _players;
}

const std::unordered_map<int, Egg> &GameState::eggs() const
{
    return _eggs;
}

bool GameState::isValidSize(int width, int height) const
{
    return width > 0 && height > 0;
}

bool GameState::isValidPosition(int x, int y) const
{
    return x >= 0 && y >= 0 && x < _width && y < _height;
}

std::size_t GameState::tileIndex(int x, int y) const
{
    return static_cast<std::size_t>(y * _width + x);
}

bool GameState::isReady() const
{
    return _width > 0 && _height > 0 && !_tiles.empty();
}

const std::vector<Tile> &GameState::tiles() const
{
    return _tiles;
}

bool GameState::startIncantation(
    int x,
    int y,
    int level,
    const std::vector<int> &playerIds
)
{
    if (tileAt(x, y) == nullptr)
        return false;

    _incantations.push_back({x, y, level, playerIds});
    return true;
}

bool GameState::endIncantation(int x, int y)
{
    auto it = std::remove_if(
        _incantations.begin(),
        _incantations.end(),
        [x, y](const Incantation &incantation) {
            return incantation.x == x && incantation.y == y;
        }
    );

    if (it == _incantations.end())
        return false;

    _incantations.erase(it, _incantations.end());
    return true;
}

const std::vector<Incantation> &GameState::incantations() const
{
    return _incantations;
}