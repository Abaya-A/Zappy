#pragma once

#include "state/GameState.hpp"
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

    bool applyEvent(const EggNewEvent &event);
    bool applyEvent(const EggHatchEvent &event);
    bool applyEvent(const EggDeathEvent &event);
    bool applyEvent(const PlayerForkEvent &event);

    bool applyEvent(const PlayerDropResourceEvent &event);
    bool applyEvent(const PlayerCollectResourceEvent &event);

    bool applyEvent(const PlayerExpulsionEvent &event);
    bool applyEvent(const PlayerBroadcastEvent &event);

    bool applyEvent(const IncantationStartEvent &event);
    bool applyEvent(const IncantationEndEvent &event);

    bool applyEvent(const UnknownCommandEvent &event);
    bool applyEvent(const BadParameterEvent &event);

    GameState &_state;
};