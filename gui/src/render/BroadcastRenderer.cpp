#include "render/BroadcastRenderer.hpp"

#include "render/RenderGeometry.hpp"
#include "render/RenderMeshFactory.hpp"

#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>

#include <vector>

namespace {

void appendBroadcastDiamond(
    std::vector<zappy::render::Vertex> &vertices,
    int x,
    int y
)
{
    constexpr float Radius = 0.42f;

    const float centerX = static_cast<float>(x) + 0.5f;
    const float centerY = static_cast<float>(y) + 0.5f;

    vertices.push_back({{centerX, centerY + Radius}});
    vertices.push_back({{centerX + Radius, centerY}});

    vertices.push_back({{centerX + Radius, centerY}});
    vertices.push_back({{centerX, centerY - Radius}});

    vertices.push_back({{centerX, centerY - Radius}});
    vertices.push_back({{centerX - Radius, centerY}});

    vertices.push_back({{centerX - Radius, centerY}});
    vertices.push_back({{centerX, centerY + Radius}});
}

std::vector<zappy::render::Vertex> buildBroadcastVertices(const GameState &state)
{
    std::vector<zappy::render::Vertex> vertices;

    for (const Broadcast &broadcast : state.broadcasts())
        appendBroadcastDiamond(vertices, broadcast.x(), broadcast.y());

    return vertices;
}

}

namespace zappy::render {

BroadcastRenderer::BroadcastRenderer(Magnum::Shaders::FlatGL2D &shader)
    : _shader(shader)
{
}

void BroadcastRenderer::draw(const GameState &state, const Magnum::Matrix3 &projection)
{
    if (state.width() <= 0 || state.height() <= 0)
        return;

    const std::vector<Vertex> vertices = buildBroadcastVertices(state);

    if (vertices.empty())
        return;

    Magnum::GL::Mesh mesh =
        RenderMeshFactory::build(Magnum::GL::MeshPrimitive::Lines, vertices);

    _shader
        .setColor(Magnum::Color4{0.25f, 1.0f, 1.0f, 1.0f})
        .setTransformationProjectionMatrix(projection)
        .draw(mesh);
}

}