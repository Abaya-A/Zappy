#pragma once

#include "GameState.hpp"
#include "GuiProtocolEvent.hpp"

class GuiEventApplier {
public:
    explicit GuiEventApplier(GameState &state);

    bool apply(const GuiProtocolEvent &event);

private:
    bool applyEvent(const MapSizeEvent &event);
    bool applyEvent(const TileContentEvent &event);
    bool applyEvent(const TeamNameEvent &event);
    bool applyEvent(const TimeUnitEvent &event);
    bool applyEvent(const EndGameEvent &event);
    bool applyEvent(const ServerMessageEvent &event);

    bool applyEvent(const PlayerNewEvent &event);
    bool applyEvent(const PlayerPositionEvent &event);
    bool applyEvent(const PlayerLevelEvent &event);
    bool applyEvent(const PlayerInventoryEvent &event);
    bool applyEvent(const PlayerDeathEvent &event);

    GameState &_state;
};