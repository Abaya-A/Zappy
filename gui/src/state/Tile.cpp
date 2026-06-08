#include "state/Tile.hpp"

#include <numeric>

Tile::Tile()
    : _resources{}
{
}

const Tile::ResourceArray &Tile::resources() const
{
    return _resources;
}

int Tile::quantity(ResourceType type) const
{
    return _resources[Resource::index(type)];
}

bool Tile::setQuantity(ResourceType type, int quantity)
{
    if (!isValidQuantity(quantity))
        return false;

    _resources[Resource::index(type)] = quantity;
    return true;
}

bool Tile::setResources(const ResourceArray &resources)
{
    if (!areValidResources(resources))
        return false;

    _resources = resources;
    return true;
}

int Tile::totalResources() const
{
    return std::accumulate(_resources.begin(), _resources.end(), 0);
}

bool Tile::isEmpty() const
{
    return totalResources() == 0;
}

bool Tile::isValidQuantity(int quantity)
{
    return quantity >= 0;
}

bool Tile::areValidResources(const ResourceArray &resources)
{
    for (int quantity : resources) {
        if (!isValidQuantity(quantity))
            return false;
    }
    return true;
}