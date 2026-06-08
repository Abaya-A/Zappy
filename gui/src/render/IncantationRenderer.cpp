#include "render/IncantationRenderer.hpp"

#include "render/RenderGeometry.hpp"
#include "render/RenderMeshFactory.hpp"

#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>

#include <vector>

namespace {

void appendIncantationOutline(
    std::vector<zappy::render::Vertex> &vertices,
    int x,
    int y
)
{
    constexpr float Padding = 0.08f;

    const float left = static_cast<float>(x) + Padding;
    const float right = static_cast<float>(x + 1) - Padding;
    const float bottom = static_cast<float>(y) + Padding;
    const float top = static_cast<float>(y + 1) - Padding;

    vertices.push_back({{left, bottom}});
    vertices.push_back({{right, bottom}});

    vertices.push_back({{right, bottom}});
    vertices.push_back({{right, top}});

    vertices.push_back({{right, top}});
    vertices.push_back({{left, top}});

    vertices.push_back({{left, top}});
    vertices.push_back({{left, bottom}});
}

std::vector<zappy::render::Vertex> buildIncantationVertices(const GameState &state)
{
    std::vector<zappy::render::Vertex> vertices;

    for (const Incantation &incantation : state.incantations())
        appendIncantationOutline(vertices, incantation.x, incantation.y);

    return vertices;
}

}

namespace zappy::render {

IncantationRenderer::IncantationRenderer(Magnum::Shaders::FlatGL2D &shader)
    : _shader(shader)
{
}

void IncantationRenderer::draw(const GameState &state, const Magnum::Matrix3 &projection)
{
    if (state.width() <= 0 || state.height() <= 0)
        return;

    const std::vector<Vertex> vertices = buildIncantationVertices(state);

    if (vertices.empty())
        return;

    Magnum::GL::Mesh mesh =
        RenderMeshFactory::build(Magnum::GL::MeshPrimitive::Lines, vertices);

    _shader
        .setColor(Magnum::Color4{1.0f, 0.95f, 0.20f, 1.0f})
        .setTransformationProjectionMatrix(projection)
        .draw(mesh);
}

}