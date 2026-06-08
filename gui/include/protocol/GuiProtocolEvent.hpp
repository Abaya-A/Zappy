#pragma once

#include "state/Player.hpp"
#include "state/Tile.hpp"

#include <string>
#include <variant>
#include <vector>

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

struct EggNewEvent {
    int id;
    int playerId;
    int x;
    int y;
};

struct EggHatchEvent {
    int id;
};

struct EggDeathEvent {
    int id;
};

struct PlayerForkEvent {
    int playerId;
};

struct PlayerDropResourceEvent {
    int playerId;
    int resourceId;
};

struct PlayerCollectResourceEvent {
    int playerId;
    int resourceId;
};

struct PlayerExpulsionEvent {
    int playerId;
};

struct PlayerBroadcastEvent {
    int playerId;
    std::string message;
};

struct IncantationStartEvent {
    int x;
    int y;
    int level;
    std::vector<int> playerIds;
};

struct IncantationEndEvent {
    int x;
    int y;
    int result;
};

struct UnknownCommandEvent {
};

struct BadParameterEvent {
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
    PlayerDeathEvent,
    EggNewEvent,
    EggHatchEvent,
    EggDeathEvent,
    PlayerForkEvent,
    PlayerDropResourceEvent,
    PlayerCollectResourceEvent,
    PlayerExpulsionEvent,
    PlayerBroadcastEvent,
    IncantationStartEvent,
    IncantationEndEvent,
    UnknownCommandEvent,
    BadParameterEvent
>;