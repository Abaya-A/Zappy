#pragma once

#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Vector2.h>

namespace zappy::render3d {

class RenderCamera3D {
public:
    Magnum::Matrix4 projection(
        int mapWidth,
        int mapHeight,
        const Magnum::Vector2i &framebufferSize
    ) const;
};

}