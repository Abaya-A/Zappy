#pragma once

#include "Player.hpp"
#include "Tile.hpp"

#include <string>
#include <variant>

struct MapSizeEvent {
    int width;
    int height;
};

struct TileContentEvent {
    int x;
    int y;
    Tile::ResourceArray resources;
};

struct TeamNameEvent {
    std::string name;
};

struct TimeUnitEvent {
    int timeUnit;
};

struct EndGameEvent {
    std::string teamName;
};

struct ServerMessageEvent {
    std::string message;
};

struct PlayerNewEvent {
    int id;
    int x;
    int y;
    int orientation;
    int level;
    std::string teamName;
};

struct PlayerPositionEvent {
    int id;
    int x;
    int y;
    int orientation;
};

struct PlayerLevelEvent {
    int id;
    int level;
};

struct PlayerInventoryEvent {
    int id;
    int x;
    int y;
    Player::Inventory inventory;
};

struct PlayerDeathEvent {
    int id;
};

using GuiProtocolEvent = std::variant<
    MapSizeEvent,
    TileContentEvent,
    TeamNameEvent,
    TimeUnitEvent,
    EndGameEvent,
    ServerMessageEvent,
    PlayerNewEvent,
    PlayerPositionEvent,
    PlayerLevelEvent,
    PlayerInventoryEvent,
    PlayerDeathEvent
>;