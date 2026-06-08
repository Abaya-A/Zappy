#include "state/Resource.hpp"

#include <algorithm>

const std::array<std::string_view, Resource::COUNT> Resource::NAMES = {
    "food",
    "linemate",
    "deraumere",
    "sibur",
    "mendiane",
    "phiras",
    "thystame"
};

std::size_t Resource::index(ResourceType type)
{
    return static_cast<std::size_t>(type);
}

std::string_view Resource::name(ResourceType type)
{
    return NAMES[index(type)];
}

std::optional<ResourceType> Resource::fromIndex(std::size_t index)
{
    if (index >= COUNT)
        return std::nullopt;
    return static_cast<ResourceType>(index);
}

std::optional<ResourceType> Resource::fromName(std::string_view name)
{
    auto it = std::find(NAMES.begin(), NAMES.end(), name);

    if (it == NAMES.end())
        return std::nullopt;
    return static_cast<ResourceType>(std::distance(NAMES.begin(), it));
}