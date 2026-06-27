#include "render/RenderCamera.hpp"

#include <algorithm>

namespace zappy::render {
namespace {

constexpr float ViewPaddingRatio = 0.1f;
constexpr float MinimumViewPadding = 0.75f;

float mapPadding(int width, int height)
{
    const float largestAxis = static_cast<float>(std::max(width, height));

    return std::max(MinimumViewPadding, largestAxis * ViewPaddingRatio);
}

Magnum::Vector2 paddedViewSize(float mapWidth, float mapHeight, float padding)
{
    return {
        mapWidth + padding * 2.0f,
        mapHeight + padding * 2.0f
    };
}

Magnum::Vector2 aspectFittedViewSize(
    const Magnum::Vector2 &targetSize,
    const Magnum::Vector2i &framebuffer
)
{
    if (framebuffer.x() <= 0 || framebuffer.y() <= 0)
        return targetSize;

    const float framebufferAspect =
        static_cast<float>(framebuffer.x()) / static_cast<float>(framebuffer.y());

    const float targetAspect = targetSize.x() / targetSize.y();

    if (framebufferAspect > targetAspect)
        return {
            targetSize.y() * framebufferAspect,
            targetSize.y()
        };

    return {
        targetSize.x(),
        targetSize.x() / framebufferAspect
    };
}

}

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

    const float padding = mapPadding(width, height);

    const Magnum::Vector2 targetSize =
        paddedViewSize(mapWidth, mapHeight, padding);

    const Magnum::Vector2 viewSize =
        aspectFittedViewSize(targetSize, framebuffer);

    return Magnum::Matrix3::projection(viewSize) *
        Magnum::Matrix3::translation({
            -mapWidth / 2.0f,
            -mapHeight / 2.0f
        });
}

}