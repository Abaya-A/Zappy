#include "render/3d/MapRenderer3D.hpp"

#include "render/3d/RenderGeometry3D.hpp"
#include "render/3d/RenderMeshFactory3D.hpp"

#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>

#include <vector>

namespace zappy::render3d {
namespace {

constexpr float GroundHeight = 0.0f;

void addLine(
    std::vector<Vertex3D> &vertices,
    const Magnum::Vector3 &from,
    const Magnum::Vector3 &to
)
{
    vertices.push_back({from});
    vertices.push_back({to});
}

std::vector<Vertex3D> buildGridVertices(int width, int height)
{
    std::vector<Vertex3D> vertices;

    if (width <= 0 || height <= 0)
        return vertices;

    vertices.reserve(static_cast<std::size_t>((width + height + 2) * 2));

    for (int x = 0; x <= width; ++x) {
        const float fx = static_cast<float>(x);

        addLine(
            vertices,
            Magnum::Vector3{fx, GroundHeight, 0.0f},
            Magnum::Vector3{fx, GroundHeight, static_cast<float>(height)}
        );
    }

    for (int z = 0; z <= height; ++z) {
        const float fz = static_cast<float>(z);

        addLine(
            vertices,
            Magnum::Vector3{0.0f, GroundHeight, fz},
            Magnum::Vector3{static_cast<float>(width), GroundHeight, fz}
        );
    }

    return vertices;
}

}

MapRenderer3D::MapRenderer3D(Magnum::Shaders::FlatGL3D &shader)
    : _shader(shader)
{
}

void MapRenderer3D::draw(const GameState &state, const Magnum::Matrix4 &projection)
{
    const std::vector<Vertex3D> vertices = buildGridVertices(
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
        .setColor(Magnum::Color4{1.0f, 1.0f, 1.0f, 1.0f})
        .draw(mesh);
}

}