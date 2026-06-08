#include "render/PlayerRenderer.hpp"

#include "render/RenderGeometry.hpp"
#include "render/RenderMeshFactory.hpp"

#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>

namespace zappy::render {

PlayerRenderer::PlayerRenderer(Magnum::Shaders::FlatGL2D &shader)
    : _shader(shader)
{
}

void PlayerRenderer::draw(const GameState &state, const Magnum::Matrix3 &projection)
{
    if (state.width() <= 0 || state.height() <= 0)
        return;

    const std::vector<Vertex> vertices = buildPlayerVertices(state);

    if (vertices.empty())
        return;

    Magnum::GL::Mesh mesh =
        RenderMeshFactory::build(Magnum::GL::MeshPrimitive::Triangles, vertices);

    _shader
        .setColor(Magnum::Color4{0.20f, 0.45f, 1.00f, 1.0f})
        .setTransformationProjectionMatrix(projection)
        .draw(mesh);
}

}