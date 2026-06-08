#include "GuiEventApplier.hpp"

#include "Player.hpp"
#include "Tile.hpp"
#include "Egg.hpp"
#include "Resource.hpp"

#include <iostream>
#include <variant>

GuiEventApplier::GuiEventApplier(GameState &state)
    : _state(state)
{
}

bool GuiEventApplier::apply(const GuiProtocolEvent &event)
{
    return std::visit([this](const auto &typedEvent) {
        return applyEvent(typedEvent);
    }, event);
}

bool GuiEventApplier::applyEvent(const MapSizeEvent &event)
{
    const bool ok = _state.setMapSize(event.width, event.height);

    if (ok)
        std::cout << "event: map size " << _state.width() << "x" << _state.height() << std::endl;
    return ok;
}

bool GuiEventApplier::applyEvent(const TileContentEvent &event)
{
    Tile tile;

    if (!tile.setResources(event.resources))
        return false;

    const bool ok = _state.setTileContent(event.x, event.y, tile);

    if (ok)
        std::cout << "event: tile " << event.x << "," << event.y << " updated" << std::endl;
    return ok;
}

bool GuiEventApplier::applyEvent(const TeamNameEvent &event)
{
    const bool ok = _state.addTeam(event.name);

    if (ok)
        std::cout << "event: team " << event.name << std::endl;
    return ok;
}

bool GuiEventApplier::applyEvent(const TimeUnitEvent &event)
{
    const bool ok = _state.setTimeUnit(event.timeUnit);

    if (ok)
        std::cout << "event: time unit " << _state.timeUnit() << std::endl;
    return ok;
}

bool GuiEventApplier::applyEvent(const EndGameEvent &event)
{
    _state.setWinner(event.teamName);

    std::cout << "event: end game winner=" << event.teamName << std::endl;
    return true;
}

bool GuiEventApplier::applyEvent(const ServerMessageEvent &event)
{
    std::cout << "event: server message " << event.message << std::endl;
    return true;
}

bool GuiEventApplier::applyEvent(const PlayerNewEvent &event)
{
    const bool ok = _state.addPlayer(Player(
        event.id,
        event.x,
        event.y,
        event.orientation,
        event.level,
        event.teamName
    ));

    if (ok)
        std::cout << "event: player #" << event.id
                  << " joined team " << event.teamName
                  << " at " << event.x << "," << event.y
                  << " orientation=" << event.orientation
                  << " level=" << event.level
                  << std::endl;
    return ok;
}

bool GuiEventApplier::applyEvent(const PlayerPositionEvent &event)
{
    const bool ok = _state.updatePlayerPosition(
        event.id,
        event.x,
        event.y,
        event.orientation
    );

    if (ok)
        std::cout << "event: player #" << event.id
                  << " moved to " << event.x << "," << event.y
                  << " orientation=" << event.orientation
                  << std::endl;
    return ok;
}

bool GuiEventApplier::applyEvent(const PlayerLevelEvent &event)
{
    const bool ok = _state.updatePlayerLevel(event.id, event.level);

    if (ok)
        std::cout << "event: player #" << event.id
                  << " level=" << event.level
                  << std::endl;
    return ok;
}

bool GuiEventApplier::applyEvent(const PlayerInventoryEvent &event)
{
    const bool ok = _state.updatePlayerInventory(
        event.id,
        event.x,
        event.y,
        event.inventory
    );

    if (ok)
        std::cout << "event: player #" << event.id
                  << " inventory updated at "
                  << event.x << "," << event.y
                  << std::endl;
    return ok;
}

bool GuiEventApplier::applyEvent(const PlayerDeathEvent &event)
{
    const bool ok = _state.removePlayer(event.id);

    if (ok)
        std::cout << "event: player #" << event.id  << " died" << std::endl;
    return ok;
}

bool GuiEventApplier::applyEvent(const EggNewEvent &event)
{
    const bool ok = _state.addEgg(Egg(
        event.id,
        event.playerId,
        event.x,
        event.y
    ));

    if (ok)
        std::cout << "event: egg #" << event.id
                  << " laid by player #" << event.playerId
                  << " at " << event.x << "," << event.y
                  << std::endl;
    return ok;
}

bool GuiEventApplier::applyEvent(const EggHatchEvent &event)
{
    const bool ok = _state.removeEgg(event.id);

    if (ok)
        std::cout << "event: egg #" << event.id
                  << " hatched"
                  << std::endl;
    return ok;
}

bool GuiEventApplier::applyEvent(const EggDeathEvent &event)
{
    const bool ok = _state.removeEgg(event.id);

    if (ok)
        std::cout << "event: egg #" << event.id
                  << " died"
                  << std::endl;
    return ok;
}

bool GuiEventApplier::applyEvent(const PlayerForkEvent &event)
{
    std::cout << "event: player #" << event.playerId
              << " is laying an egg"
              << std::endl;
    return true;
}

bool GuiEventApplier::applyEvent(const PlayerDropResourceEvent &event)
{
    std::cout << "event: player #" << event.playerId
              << " dropped resource " << event.resourceId
              << std::endl;
    return true;
}

bool GuiEventApplier::applyEvent(const PlayerCollectResourceEvent &event)
{
    std::cout << "event: player #" << event.playerId
              << " collected resource " << event.resourceId
              << std::endl;
    return true;
}