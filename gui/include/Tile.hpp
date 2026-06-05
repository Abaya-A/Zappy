#pragma once

#include "Resource.hpp"

#include <array>
#include <optional>

class Tile {
public:
    using ResourceArray = std::array<int, Resource::COUNT>;

    Tile();

    const ResourceArray &resources() const;

    int quantity(ResourceType type) const;
    bool setQuantity(ResourceType type, int quantity);

    bool setResources(const ResourceArray &resources);
    int totalResources() const;
    bool isEmpty() const;

private:
    ResourceArray _resources;

    static bool isValidQuantity(int quantity);
    static bool areValidResources(const ResourceArray &resources);
};