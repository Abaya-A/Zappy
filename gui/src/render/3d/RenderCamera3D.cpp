#include "render/3d/RenderCamera3D.hpp"

#include <Magnum/Math/Functions.h>
#include <Magnum/Math/Matrix4.h>

namespace zappy::render3d {

Magnum::Matrix4 RenderCamera3D::projection(
    int mapWidth,
    int mapHeight,
    const Magnum::Vector2i &framebufferSize
) const
{
    const float width = static_cast<float>(mapWidth > 0 ? mapWidth : 1);
    const float height = static_cast<float>(mapHeight > 0 ? mapHeight : 1);
    const float aspect = framebufferSize.y() == 0
        ? 1.0f
        : static_cast<float>(framebufferSize.x()) / static_cast<float>(framebufferSize.y());

    const Magnum::Matrix4 perspective =
        Magnum::Matrix4::perspectiveProjection(
            Magnum::Deg{45.0f},
            aspect,
            0.1f,
            100.0f
        );

    const Magnum::Vector3 center{width * 0.5f, 0.0f, height * 0.5f};

    const Magnum::Matrix4 view =
        Magnum::Matrix4::lookAt(
            Magnum::Vector3{width * 0.5f, -height * 1.4f, height * 1.2f + 6.0f},
            center,
            Magnum::Vector3{0.0f, 0.0f, 1.0f}
        );

    return perspective * view;
}

}