#pragma once

#include "GuiProtocolEvent.hpp"
#include "ProtocolCommand.hpp"

#include <optional>

class GuiProtocolDecoder {
public:
    std::optional<GuiProtocolEvent> decode(const ProtocolCommand &command) const;

private:
    std::optional<GuiProtocolEvent> decodeMapSize(const ProtocolCommand &command) const;
    std::optional<GuiProtocolEvent> decodeTileContent(const ProtocolCommand &command) const;
    std::optional<GuiProtocolEvent> decodeTeamName(const ProtocolCommand &command) const;
    std::optional<GuiProtocolEvent> decodeTimeUnit(const ProtocolCommand &command) const;
    std::optional<GuiProtocolEvent> decodeEndGame(const ProtocolCommand &command) const;
    std::optional<GuiProtocolEvent> decodeServerMessage(const ProtocolCommand &command) const;

    std::optional<GuiProtocolEvent> decodePlayerNew(const ProtocolCommand &command) const;
    std::optional<GuiProtocolEvent> decodePlayerPosition(const ProtocolCommand &command) const;
    std::optional<GuiProtocolEvent> decodePlayerLevel(const ProtocolCommand &command) const;
    std::optional<GuiProtocolEvent> decodePlayerInventory(const ProtocolCommand &command) const;
    std::optional<GuiProtocolEvent> decodePlayerDeath(const ProtocolCommand &command) const;

    std::optional<Tile::ResourceArray> parseTileResources(
        const ProtocolCommand &command,
        std::size_t startIndex
    ) const;

    std::optional<Player::Inventory> parsePlayerInventory(
        const ProtocolCommand &command,
        std::size_t startIndex
    ) const;
};