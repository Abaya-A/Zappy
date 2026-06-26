#include "render/3d/RenderCamera3D.hpp"

#include <Magnum/Math/Angle.h>
#include <Magnum/Math/Matrix4.h>

#include <algorithm>

namespace zappy::render3d {

Magnum::Matrix4 RenderCamera3D::projection(
    int mapWidth,
    int mapHeight,
    const Magnum::Vector2i &framebufferSize
) const
{
    const float width = static_cast<float>(mapWidth > 0 ? mapWidth : 1);
    const float depth = static_cast<float>(mapHeight > 0 ? mapHeight : 1);
    const float mapSize = std::max(width, depth);

    const float aspect = framebufferSize.y() == 0
        ? 1.0f
        : static_cast<float>(framebufferSize.x()) / static_cast<float>(framebufferSize.y());

    const Magnum::Vector3 center{
        width * 0.5f,
        0.0f,
        depth * 0.5f
    };

    const Magnum::Vector3 eye{
        width * 0.5f,
        mapSize * 1.2f + 6.0f,
        depth * 1.4f + 6.0f
    };

    const Magnum::Matrix4 projection =
        Magnum::Matrix4::perspectiveProjection(
            Magnum::Deg{45.0f},
            aspect,
            0.1f,
            200.0f
        );

const Magnum::Matrix4 camera =
    Magnum::Matrix4::lookAt(
        eye,
        center,
        Magnum::Vector3::yAxis()
    );

    return projection * camera.inverted();
}

}