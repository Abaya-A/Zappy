#include "render/RenderGeometry.hpp"

#include "state/Tile.hpp"

#include <cstddef>

namespace zappy::render {

const std::array<ResourceMarkerStyle, ResourceCount> ResourceMarkerStyles = {{
    {Magnum::Color4{0.35f, 0.95f, 0.35f, 1.0f}, 0.18f, 0.18f},
    {Magnum::Color4{0.40f, 0.75f, 1.00f, 1.0f}, 0.42f, 0.18f},
    {Magnum::Color4{0.85f, 0.65f, 1.00f, 1.0f}, 0.66f, 0.18f},
    {Magnum::Color4{1.00f, 0.85f, 0.35f, 1.0f}, 0.18f, 0.42f},
    {Magnum::Color4{1.00f, 0.55f, 0.35f, 1.0f}, 0.42f, 0.42f},
    {Magnum::Color4{0.95f, 0.35f, 0.75f, 1.0f}, 0.66f, 0.42f},
    {Magnum::Color4{0.90f, 0.90f, 0.95f, 1.0f}, 0.42f, 0.66f},
}};

std::vector<Vertex> buildGridVertices(int width, int height)
{
    std::vector<Vertex> vertices;

    vertices.reserve(static_cast<std::size_t>((width + height + 2) * 2));

    for (int x = 0; x <= width; ++x) {
        const float xf = static_cast<float>(x);

        vertices.push_back({{xf, 0.0f}});
        vertices.push_back({{xf, static_cast<float>(height)}});
    }

    for (int y = 0; y <= height; ++y) {
        const float yf = static_cast<float>(y);

        vertices.push_back({{0.0f, yf}});
        vertices.push_back({{static_cast<float>(width), yf}});
    }

    return vertices;
}

static void appendSquareMarker(
    std::vector<Vertex> &vertices,
    int tileX,
    int tileY,
    float offsetX,
    float offsetY
)
{
    constexpr float MarkerSize = 0.16f;

    const float left = static_cast<float>(tileX) + offsetX;
    const float right = left + MarkerSize;
    const float bottom = static_cast<float>(tileY) + offsetY;
    const float top = bottom + MarkerSize;

    vertices.push_back({{left, bottom}});
    vertices.push_back({{right, bottom}});
    vertices.push_back({{right, top}});

    vertices.push_back({{left, bottom}});
    vertices.push_back({{right, top}});
    vertices.push_back({{left, top}});
}

std::array<std::vector<Vertex>, ResourceCount> buildResourceVertices(const GameState &state)
{
    std::array<std::vector<Vertex>, ResourceCount> verticesByResource;

    for (int y = 0; y < state.height(); ++y) {
        for (int x = 0; x < state.width(); ++x) {
            const Tile *tile = state.tileAt(x, y);

            if (tile == nullptr)
                continue;

            const Tile::ResourceArray &resources = tile->resources();

            for (int resourceId = 0; resourceId < ResourceCount; ++resourceId) {
                if (resources[resourceId] <= 0)
                    continue;

                const ResourceMarkerStyle &style = ResourceMarkerStyles[resourceId];

                appendSquareMarker(
                    verticesByResource[resourceId],
                    x,
                    y,
                    style.offsetX,
                    style.offsetY
                );
            }
        }
    }

    return verticesByResource;
}

static void appendEggDiamond(std::vector<Vertex> &vertices, int x, int y)
{
    const float centerX = static_cast<float>(x) + 0.5f;
    const float centerY = static_cast<float>(y) + 0.5f;

    constexpr float Radius = 0.18f;

    const Magnum::Vector2 top{centerX, centerY + Radius};
    const Magnum::Vector2 right{centerX + Radius, centerY};
    const Magnum::Vector2 bottom{centerX, centerY - Radius};
    const Magnum::Vector2 left{centerX - Radius, centerY};

    vertices.push_back({top});
    vertices.push_back({right});
    vertices.push_back({bottom});

    vertices.push_back({top});
    vertices.push_back({bottom});
    vertices.push_back({left});
}

std::vector<Vertex> buildEggVertices(const GameState &state)
{
    std::vector<Vertex> vertices;

    for (const auto &[id, egg] : state.eggs()) {
        (void)id;
        appendEggDiamond(vertices, egg.x(), egg.y());
    }

    return vertices;
}

static void appendPlayerTriangle(
    std::vector<Vertex> &vertices,
    int x,
    int y,
    int orientation
)
{
    const float centerX = static_cast<float>(x) + 0.5f;
    const float centerY = static_cast<float>(y) + 0.5f;

    constexpr float Front = 0.32f;
    constexpr float Back = 0.22f;
    constexpr float HalfWidth = 0.22f;

    switch (orientation) {
        case 1:
            vertices.push_back({{centerX, centerY + Front}});
            vertices.push_back({{centerX - HalfWidth, centerY - Back}});
            vertices.push_back({{centerX + HalfWidth, centerY - Back}});
            break;
        case 2:
            vertices.push_back({{centerX + Front, centerY}});
            vertices.push_back({{centerX - Back, centerY - HalfWidth}});
            vertices.push_back({{centerX - Back, centerY + HalfWidth}});
            break;
        case 3:
            vertices.push_back({{centerX, centerY - Front}});
            vertices.push_back({{centerX + HalfWidth, centerY + Back}});
            vertices.push_back({{centerX - HalfWidth, centerY + Back}});
            break;
        case 4:
            vertices.push_back({{centerX - Front, centerY}});
            vertices.push_back({{centerX + Back, centerY + HalfWidth}});
            vertices.push_back({{centerX + Back, centerY - HalfWidth}});
            break;
        default:
            vertices.push_back({{centerX, centerY + Front}});
            vertices.push_back({{centerX - HalfWidth, centerY - Back}});
            vertices.push_back({{centerX + HalfWidth, centerY - Back}});
            break;
    }
}

std::vector<Vertex> buildPlayerVertices(const GameState &state)
{
    std::vector<Vertex> vertices;

    for (const auto &[id, player] : state.players()) {
        (void)id;
        appendPlayerTriangle(
            vertices,
            player.x(),
            player.y(),
            player.orientation()
        );
    }

    return vertices;
}

}