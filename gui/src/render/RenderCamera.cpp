#include "render/RenderCamera.hpp"

namespace zappy::render {

Magnum::Matrix3 RenderCamera::projection(
    int width,
    int height,
    const Magnum::Vector2i &framebuffer
) const
{
    const float mapWidth = static_cast<float>(width);
    const float mapHeight = static_cast<float>(height);

    if (mapWidth <= 0.0f || mapHeight <= 0.0f)
        return Magnum::Matrix3::projection({1.0f, 1.0f});

    if (framebuffer.x() <= 0 || framebuffer.y() <= 0)
        return Magnum::Matrix3::projection({mapWidth, mapHeight});

    const float framebufferAspect =
        static_cast<float>(framebuffer.x()) / static_cast<float>(framebuffer.y());
    const float mapAspect = mapWidth / mapHeight;

    float viewWidth = mapWidth;
    float viewHeight = mapHeight;

    if (framebufferAspect > mapAspect) {
        viewHeight = mapHeight;
        viewWidth = mapHeight * framebufferAspect;
    } else {
        viewWidth = mapWidth;
        viewHeight = mapWidth / framebufferAspect;
    }

    return Magnum::Matrix3::projection({viewWidth, viewHeight}) *
           Magnum::Matrix3::translation({-mapWidth / 2.0f, -mapHeight / 2.0f});
}

}