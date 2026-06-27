#pragma once

#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Range.h>
#include <Magnum/Math/Vector2.h>

#include <optional>

namespace zappy::render {

class MinimapTilePicker {
public:
    void setPickingArea(
        const Magnum::Range2Di &viewport,
        const Magnum::Matrix3 &projection
    );

    std::optional<Magnum::Vector2i> pickTile(
        const Magnum::Vector2i &framebufferPosition,
        int mapWidth,
        int mapHeight
    ) const;

private:
    bool contains(const Magnum::Vector2i &position) const;

    Magnum::Range2Di _viewport;
    Magnum::Matrix3 _projection;
};

}