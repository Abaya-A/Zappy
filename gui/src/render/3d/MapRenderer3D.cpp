#include "render/3d/MapRenderer3D.hpp"

#include "render/3d/RenderGeometry3D.hpp"
#include "render/3d/RenderMeshFactory3D.hpp"

#include <Magnum/Math/Color.h>

#include <vector>

namespace zappy::render3d {

MapRenderer3D::MapRenderer3D(Magnum::Shaders::FlatGL3D &shader)
    : _shader(shader)
{
}

void MapRenderer3D::draw(const GameState &state, const Magnum::Matrix4 &projection)
{
    std::vector<Vertex3D> vertices;

    const int width = state.width();
    const int height = state.height();

    for (int x = 0; x <= width; ++x) {
        vertices.push_back({Magnum::Vector3{static_cast<float>(x), 0.0f, 0.0f}});
        vertices.push_back({Magnum::Vector3{static_cast<float>(x), 0.0f, static_cast<float>(height)}});
    }

    for (int y = 0; y <= height; ++y) {
        vertices.push_back({Magnum::Vector3{0.0f, 0.0f, static_cast<float>(y)}});
        vertices.push_back({Magnum::Vector3{static_cast<float>(width), 0.0f, static_cast<float>(y)}});
    }

    if (vertices.empty())
        return;

    Magnum::GL::Mesh mesh = RenderMeshFactory3D::build(MeshPrimitive3D::Lines, vertices);

    _shader
        .setTransformationProjectionMatrix(projection)
        .setColor(Magnum::Color4{0.35f, 0.35f, 0.40f, 1.0f})
        .draw(mesh);
}

}