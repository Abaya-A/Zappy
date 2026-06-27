#pragma once

#include <Magnum/Math/Vector2.h>

#include <optional>

namespace zappy::render {

class TileSelection {
public:
    void select(const Magnum::Vector2i &tilePosition);
    void clear();

    bool hasSelection() const;
    const std::optional<Magnum::Vector2i> &selectedTile() const;

private:
    std::optional<Magnum::Vector2i> _selectedTile;
};

}