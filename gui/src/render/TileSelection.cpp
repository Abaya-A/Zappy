#include "render/TileSelection.hpp"

namespace zappy::render {

void TileSelection::select(const Magnum::Vector2i &tilePosition)
{
    _selectedTile = tilePosition;
}

void TileSelection::clear()
{
    _selectedTile = std::nullopt;
}

bool TileSelection::hasSelection() const
{
    return _selectedTile.has_value();
}

const std::optional<Magnum::Vector2i> &TileSelection::selectedTile() const
{
    return _selectedTile;
}

}