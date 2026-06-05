#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <string_view>

enum class ResourceType : std::size_t {
    Food = 0,
    Linemate = 1,
    Deraumere = 2,
    Sibur = 3,
    Mendiane = 4,
    Phiras = 5,
    Thystame = 6
};

class Resource {
public:
    static constexpr std::size_t COUNT = 7;

    static std::size_t index(ResourceType type);
    static std::string_view name(ResourceType type);
    static std::optional<ResourceType> fromIndex(std::size_t index);
    static std::optional<ResourceType> fromName(std::string_view name);

private:
    static const std::array<std::string_view, COUNT> NAMES;
};