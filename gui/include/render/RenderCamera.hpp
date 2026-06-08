#pragma once

#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Vector2.h>

namespace zappy::render {

class RenderCamera {
public:
    Magnum::Matrix3 projection(
        int mapWidth,
        int mapHeight,
        const Magnum::Vector2i &framebufferSize
    ) const;
};

}