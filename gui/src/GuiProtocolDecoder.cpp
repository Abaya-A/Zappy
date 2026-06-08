#include "GuiProtocolDecoder.hpp"

#include "Resource.hpp"

#include <sstream>

GuiProtocolDecoder::GuiProtocolDecoder()
    : _decoders()
{
    registerDecoders();
}

void GuiProtocolDecoder::registerDecoders()
{
    _decoders["msz"] = &GuiProtocolDecoder::decodeMapSize;
    _decoders["bct"] = &GuiProtocolDecoder::decodeTileContent;
    _decoders["tna"] = &GuiProtocolDecoder::decodeTeamName;
    _decoders["sgt"] = &GuiProtocolDecoder::decodeTimeUnit;
    _decoders["seg"] = &GuiProtocolDecoder::decodeEndGame;
    _decoders["smg"] = &GuiProtocolDecoder::decodeServerMessage;

    _decoders["pnw"] = &GuiProtocolDecoder::decodePlayerNew;
    _decoders["ppo"] = &GuiProtocolDecoder::decodePlayerPosition;
    _decoders["plv"] = &GuiProtocolDecoder::decodePlayerLevel;
    _decoders["pin"] = &GuiProtocolDecoder::decodePlayerInventory;
    _decoders["pdi"] = &GuiProtocolDecoder::decodePlayerDeath;
}

std::optional<GuiProtocolEvent> GuiProtocolDecoder::decode(const ProtocolCommand &command) const
{
    const auto it = _decoders.find(command.name());

    if (it == _decoders.end())
        return std::nullopt;

    return (this->*(it->second))(command);
}

std::optional<GuiProtocolEvent> GuiProtocolDecoder::decodeMapSize(const ProtocolCommand &command) const
{
    if (!command.hasArgCount(2))
        return std::nullopt;

    const auto width = command.intArg(0);
    const auto height = command.intArg(1);

    if (!width || !height)
        return std::nullopt;

    return MapSizeEvent{*width, *height};
}

std::optional<GuiProtocolEvent> GuiProtocolDecoder::decodeTileContent(const ProtocolCommand &command) const
{
    if (!command.hasArgCount(9))
        return std::nullopt;

    const auto x = command.intArg(0);
    const auto y = command.intArg(1);
    const auto resources = parseTileResources(command, 2);

    if (!x || !y || !resources)
        return std::nullopt;

    return TileContentEvent{*x, *y, *resources};
}

std::optional<GuiProtocolEvent> GuiProtocolDecoder::decodeTeamName(const ProtocolCommand &command) const
{
    if (!command.hasArgCount(1))
        return std::nullopt;

    const auto name = command.arg(0);

    if (!name)
        return std::nullopt;

    return TeamNameEvent{*name};
}

std::optional<GuiProtocolEvent> GuiProtocolDecoder::decodeTimeUnit(const ProtocolCommand &command) const
{
    if (!command.hasArgCount(1))
        return std::nullopt;

    const auto timeUnit = command.intArg(0);

    if (!timeUnit)
        return std::nullopt;

    return TimeUnitEvent{*timeUnit};
}

std::optional<GuiProtocolEvent> GuiProtocolDecoder::decodeEndGame(const ProtocolCommand &command) const
{
    if (!command.hasArgCount(1))
        return std::nullopt;

    const auto teamName = command.arg(0);

    if (!teamName)
        return std::nullopt;

    return EndGameEvent{*teamName};
}

std::optional<GuiProtocolEvent> GuiProtocolDecoder::decodeServerMessage(const ProtocolCommand &command) const
{
    std::ostringstream message;

    for (std::size_t i = 0; i < command.args().size(); ++i) {
        if (i != 0)
            message << " ";
        message << command.args()[i];
    }

    return ServerMessageEvent{message.str()};
}

std::optional<GuiProtocolEvent> GuiProtocolDecoder::decodePlayerNew(const ProtocolCommand &command) const
{
    if (!command.hasArgCount(6))
        return std::nullopt;

    const auto id = command.idArg(0);
    const auto x = command.intArg(1);
    const auto y = command.intArg(2);
    const auto orientation = command.intArg(3);
    const auto level = command.intArg(4);
    const auto teamName = command.arg(5);

    if (!id || !x || !y || !orientation || !level || !teamName)
        return std::nullopt;

    return PlayerNewEvent{*id, *x, *y, *orientation, *level, *teamName};
}

std::optional<GuiProtocolEvent> GuiProtocolDecoder::decodePlayerPosition(const ProtocolCommand &command) const
{
    if (!command.hasArgCount(4))
        return std::nullopt;

    const auto id = command.idArg(0);
    const auto x = command.intArg(1);
    const auto y = command.intArg(2);
    const auto orientation = command.intArg(3);

    if (!id || !x || !y || !orientation)
        return std::nullopt;

    return PlayerPositionEvent{*id, *x, *y, *orientation};
}

std::optional<GuiProtocolEvent> GuiProtocolDecoder::decodePlayerLevel(const ProtocolCommand &command) const
{
    if (!command.hasArgCount(2))
        return std::nullopt;

    const auto id = command.idArg(0);
    const auto level = command.intArg(1);

    if (!id || !level)
        return std::nullopt;

    return PlayerLevelEvent{*id, *level};
}

std::optional<GuiProtocolEvent> GuiProtocolDecoder::decodePlayerInventory(const ProtocolCommand &command) const
{
    if (!command.hasArgCount(10))
        return std::nullopt;

    const auto id = command.idArg(0);
    const auto x = command.intArg(1);
    const auto y = command.intArg(2);
    const auto inventory = parsePlayerInventory(command, 3);

    if (!id || !x || !y || !inventory)
        return std::nullopt;

    return PlayerInventoryEvent{*id, *x, *y, *inventory};
}

std::optional<GuiProtocolEvent> GuiProtocolDecoder::decodePlayerDeath(const ProtocolCommand &command) const
{
    if (!command.hasArgCount(1))
        return std::nullopt;

    const auto id = command.idArg(0);

    if (!id)
        return std::nullopt;

    return PlayerDeathEvent{*id};
}

std::optional<Tile::ResourceArray> GuiProtocolDecoder::parseTileResources(
    const ProtocolCommand &command,
    std::size_t startIndex
) const
{
    Tile::ResourceArray resources{};

    for (std::size_t i = 0; i < Resource::COUNT; ++i) {
        const auto quantity = command.intArg(startIndex + i);

        if (!quantity)
            return std::nullopt;

        resources[i] = *quantity;
    }

    return resources;
}

std::optional<Player::Inventory> GuiProtocolDecoder::parsePlayerInventory(
    const ProtocolCommand &command,
    std::size_t startIndex
) const
{
    Player::Inventory inventory{};

    for (std::size_t i = 0; i < Resource::COUNT; ++i) {
        const auto quantity = command.intArg(startIndex + i);

        if (!quantity)
            return std::nullopt;

        inventory[i] = *quantity;
    }

    return inventory;
}