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

void addTriangle(
    std::vector<Vertex3D> &vertices,
    const Magnum::Vector3 &a,
    const Magnum::Vector3 &b,
    const Magnum::Vector3 &c
)
{
    vertices.push_back({a});
    vertices.push_back({b});
    vertices.push_back({c});
}

void addQuad(
    std::vector<Vertex3D> &vertices,
    const Magnum::Vector3 &a,
    const Magnum::Vector3 &b,
    const Magnum::Vector3 &c,
    const Magnum::Vector3 &d
)
{
    addTriangle(vertices, a, b, c);
    addTriangle(vertices, a, c, d);
}

std::vector<Vertex3D> buildFloorVertices(int width, int height)
{
    std::vector<Vertex3D> vertices;

    if (width <= 0 || height <= 0)
        return vertices;

    vertices.reserve(static_cast<std::size_t>(width * height * 6));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const float fx = static_cast<float>(x);
            const float fz = static_cast<float>(y);

            addQuad(
                vertices,
                Magnum::Vector3{fx, GroundHeight, fz},
                Magnum::Vector3{fx + 1.0f, GroundHeight, fz},
                Magnum::Vector3{fx + 1.0f, GroundHeight, fz + 1.0f},
                Magnum::Vector3{fx, GroundHeight, fz + 1.0f}
            );
        }
    }

    return vertices;
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
            Magnum::Vector3{fx, GroundHeight + 0.01f, 0.0f},
            Magnum::Vector3{fx, GroundHeight + 0.01f, static_cast<float>(height)}
        );
    }

    for (int z = 0; z <= height; ++z) {
        const float fz = static_cast<float>(z);

        addLine(
            vertices,
            Magnum::Vector3{0.0f, GroundHeight + 0.01f, fz},
            Magnum::Vector3{static_cast<float>(width), GroundHeight + 0.01f, fz}
        );
    }

    return vertices;
}

void drawMesh(
    Magnum::Shaders::FlatGL3D &shader,
    const Magnum::Matrix4 &projection,
    MeshPrimitive3D primitive,
    const std::vector<Vertex3D> &vertices,
    const Magnum::Color4 &color
)
{
    if (vertices.empty())
        return;

    Magnum::GL::Mesh mesh = RenderMeshFactory3D::build(primitive, vertices);

    shader
        .setTransformationProjectionMatrix(projection)
        .setColor(color)
        .draw(mesh);
}

}

MapRenderer3D::MapRenderer3D(Magnum::Shaders::FlatGL3D &shader)
    : _shader(shader)
{
}

void MapRenderer3D::draw(const GameState &state, const Magnum::Matrix4 &projection)
{
    const std::vector<Vertex3D> floorVertices = buildFloorVertices(
        state.width(),
        state.height()
    );

    const std::vector<Vertex3D> gridVertices = buildGridVertices(
        state.width(),
        state.height()
    );

    drawMesh(
        _shader,
        projection,
        MeshPrimitive3D::Triangles,
        floorVertices,
        Magnum::Color4{0.10f, 0.11f, 0.15f, 1.0f}
    );

    drawMesh(
        _shader,
        projection,
        MeshPrimitive3D::Lines,
        gridVertices,
        Magnum::Color4{0.85f, 0.85f, 0.90f, 1.0f}
    );
}

}