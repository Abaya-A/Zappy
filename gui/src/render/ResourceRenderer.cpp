#include "render/ResourceRenderer.hpp"

#include "render/RenderGeometry.hpp"
#include "render/RenderMeshFactory.hpp"

#include <Magnum/GL/Mesh.h>

namespace zappy::render {

ResourceRenderer::ResourceRenderer(Magnum::Shaders::FlatGL2D &shader)
    : _shader(shader)
{
}

void ResourceRenderer::draw(const GameState &state, const Magnum::Matrix3 &projection)
{
    if (state.width() <= 0 || state.height() <= 0)
        return;

    const std::array<std::vector<Vertex>, ResourceCount> verticesByResource =
        buildResourceVertices(state);

    for (int resourceId = 0; resourceId < ResourceCount; ++resourceId) {
        const std::vector<Vertex> &vertices = verticesByResource[resourceId];

        if (vertices.empty())
            continue;

        Magnum::GL::Mesh mesh =
            RenderMeshFactory::build(Magnum::GL::MeshPrimitive::Triangles, vertices);

        _shader
            .setColor(ResourceMarkerStyles[resourceId].color)
            .setTransformationProjectionMatrix(projection)
            .draw(mesh);
    }
}

}