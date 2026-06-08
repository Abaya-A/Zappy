#include "render/MapRenderer.hpp"

#include "render/RenderGeometry.hpp"
#include "render/RenderMeshFactory.hpp"

#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>

namespace zappy::render {

MapRenderer::MapRenderer(Magnum::Shaders::FlatGL2D &shader)
    : _shader(shader)
{
}

void MapRenderer::draw(const GameState &state, const Magnum::Matrix3 &projection)
{
    if (state.width() <= 0 || state.height() <= 0)
        return;

    const std::vector<Vertex> vertices = buildGridVertices(state.width(), state.height());
    Magnum::GL::Mesh mesh = RenderMeshFactory::build(Magnum::GL::MeshPrimitive::Lines, vertices);

    _shader
        .setColor(Magnum::Color4{0.75f, 0.75f, 0.85f, 1.0f})
        .setTransformationProjectionMatrix(projection)
        .draw(mesh);
}

}