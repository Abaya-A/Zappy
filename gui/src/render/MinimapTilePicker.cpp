#include "render/MinimapTilePicker.hpp"

#include <cmath>
#include <optional>

namespace zappy::render {
namespace {

bool isValidMapSize(int width, int height)
{
    return width > 0 && height > 0;
}

Magnum::Vector2 toClipSpace(
    const Magnum::Vector2i &localPosition,
    const Magnum::Vector2i &viewportSize
)
{
    const float normalizedX =
        static_cast<float>(localPosition.x()) /
        static_cast<float>(viewportSize.x());

    const float normalizedY =
        static_cast<float>(localPosition.y()) /
        static_cast<float>(viewportSize.y());

    return {
        normalizedX * 2.0f - 1.0f,
        normalizedY * 2.0f - 1.0f
    };
}

bool isInsideMap(const Magnum::Vector2 &worldPosition, int width, int height)
{
    return worldPosition.x() >= 0.0f &&
           worldPosition.y() >= 0.0f &&
           worldPosition.x() < static_cast<float>(width) &&
           worldPosition.y() < static_cast<float>(height);
}

Magnum::Vector2i worldPositionToTile(const Magnum::Vector2 &worldPosition)
{
    return {
        static_cast<int>(std::floor(worldPosition.x())),
        static_cast<int>(std::floor(worldPosition.y()))
    };
}

}

void MinimapTilePicker::setPickingArea(
    const Magnum::Range2Di &viewport,
    const Magnum::Matrix3 &projection
)
{
    _viewport = viewport;
    _projection = projection;
}

bool MinimapTilePicker::contains(const Magnum::Vector2i &position) const
{
    return position.x() >= _viewport.min().x() &&
           position.y() >= _viewport.min().y() &&
           position.x() < _viewport.max().x() &&
           position.y() < _viewport.max().y();
}

std::optional<Magnum::Vector2i> MinimapTilePicker::pickTile(
    const Magnum::Vector2i &framebufferPosition,
    int mapWidth,
    int mapHeight
) const
{
    if (!isValidMapSize(mapWidth, mapHeight))
        return std::nullopt;

    if (!contains(framebufferPosition))
        return std::nullopt;

    const Magnum::Vector2i viewportSize = _viewport.size();

    if (viewportSize.x() <= 0 || viewportSize.y() <= 0)
        return std::nullopt;

    const Magnum::Vector2i localPosition{
        framebufferPosition.x() - _viewport.min().x(),
        framebufferPosition.y() - _viewport.min().y()
    };

    const Magnum::Vector2 clipPosition =
        toClipSpace(localPosition, viewportSize);

    const Magnum::Vector2 worldPosition =
        _projection.inverted().transformPoint(clipPosition);

    if (!isInsideMap(worldPosition, mapWidth, mapHeight))
        return std::nullopt;

    return worldPositionToTile(worldPosition);
}

}