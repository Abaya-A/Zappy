#include "render/3d/SelectedTileRenderer3D.hpp"

#include "render/3d/PlanetGeometry3D.hpp"
#include "render/3d/RenderMeshFactory3D.hpp"

#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>

#include <vector>

namespace zappy::render3d {
namespace {

constexpr float HighlightSurfaceLift = 0.10f;
constexpr float TileCornerOffset = 0.5f;

bool isValidSelectedTile(
    const Magnum::Vector2i &tile,
    int mapWidth,
    int mapHeight
)
{
    return tile.x() >= 0 &&
           tile.y() >= 0 &&
           tile.x() < mapWidth &&
           tile.y() < mapHeight;
}

void addLine(
    std::vector<Vertex3D> &vertices,
    const Magnum::Vector3 &from,
    const Magnum::Vector3 &to
)
{
    vertices.push_back({from});
    vertices.push_back({to});
}

Magnum::Vector3 tileSurfacePoint(
    float tileX,
    float tileY,
    int mapWidth,
    int mapHeight
)
{
    return PlanetGeometry3D::surfacePoint(
        tileX,
        tileY,
        mapWidth,
        mapHeight,
        HighlightSurfaceLift
    );
}

std::vector<Vertex3D> buildTileOutlineVertices(
    const Magnum::Vector2i &tile,
    int mapWidth,
    int mapHeight
)
{
    std::vector<Vertex3D> vertices;
    vertices.reserve(8);

    const float left = static_cast<float>(tile.x()) - TileCornerOffset;
    const float right = static_cast<float>(tile.x()) + TileCornerOffset;
    const float top = static_cast<float>(tile.y()) - TileCornerOffset;
    const float bottom = static_cast<float>(tile.y()) + TileCornerOffset;

    const Magnum::Vector3 topLeft =
        tileSurfacePoint(left, top, mapWidth, mapHeight);
    const Magnum::Vector3 topRight =
        tileSurfacePoint(right, top, mapWidth, mapHeight);
    const Magnum::Vector3 bottomRight =
        tileSurfacePoint(right, bottom, mapWidth, mapHeight);
    const Magnum::Vector3 bottomLeft =
        tileSurfacePoint(left, bottom, mapWidth, mapHeight);

    addLine(vertices, topLeft, topRight);
    addLine(vertices, topRight, bottomRight);
    addLine(vertices, bottomRight, bottomLeft);
    addLine(vertices, bottomLeft, topLeft);

    return vertices;
}

}

SelectedTileRenderer3D::SelectedTileRenderer3D(Magnum::Shaders::FlatGL3D &shader)
    : _shader(shader)
{
}

void SelectedTileRenderer3D::draw(
    const GameState &state,
    const Magnum::Matrix4 &projection,
    const std::optional<Magnum::Vector2i> &selectedTile
)
{
    if (!selectedTile.has_value())
        return;

    const Magnum::Vector2i &tile = selectedTile.value();

    if (!isValidSelectedTile(tile, state.width(), state.height()))
        return;

    const std::vector<Vertex3D> vertices = buildTileOutlineVertices(
        tile,
        state.width(),
        state.height()
    );

    if (vertices.empty())
        return;

    Magnum::GL::Mesh mesh = RenderMeshFactory3D::build(
        MeshPrimitive3D::Lines,
        vertices
    );

    _shader
        .setTransformationProjectionMatrix(projection)
        .setColor(Magnum::Color4{1.0f, 0.95f, 0.20f, 1.0f})
        .draw(mesh);
}

}