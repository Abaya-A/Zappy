#include "render/RenderMeshFactory.hpp"

#include <Corrade/Containers/ArrayView.h>

#include <Magnum/GL/Buffer.h>
#include <Magnum/Shaders/FlatGL.h>

namespace zappy::render {

Magnum::GL::Mesh RenderMeshFactory::build(
    Magnum::GL::MeshPrimitive primitive,
    const std::vector<Vertex> &vertices
)
{
    Magnum::GL::Buffer vertexBuffer;

    vertexBuffer.setData(
        Corrade::Containers::arrayCast<const char>(
            Corrade::Containers::arrayView(vertices.data(), vertices.size())
        )
    );

    Magnum::GL::Mesh mesh;
    mesh.setPrimitive(primitive)
        .setCount(static_cast<int>(vertices.size()))
        .addVertexBuffer(
            std::move(vertexBuffer),
            0,
            Magnum::Shaders::FlatGL2D::Position{}
        );

    return mesh;
}

}