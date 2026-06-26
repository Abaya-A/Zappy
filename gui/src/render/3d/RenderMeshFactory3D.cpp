#include "render/3d/RenderMeshFactory3D.hpp"

#include <Corrade/Containers/ArrayView.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/FlatGL.h>

namespace zappy::render3d {

Magnum::GL::Mesh RenderMeshFactory3D::build(
    MeshPrimitive3D primitive,
    const std::vector<Vertex3D> &vertices
)
{
    Magnum::GL::Buffer vertexBuffer;

    vertexBuffer.setData(
        Corrade::Containers::arrayCast<const char>(
            Corrade::Containers::arrayView(vertices.data(), vertices.size())
        )
    );

    Magnum::GL::Mesh mesh;

    if (primitive == MeshPrimitive3D::Lines)
        mesh.setPrimitive(Magnum::GL::MeshPrimitive::Lines);
    else
        mesh.setPrimitive(Magnum::GL::MeshPrimitive::Triangles);

    mesh.setCount(static_cast<int>(vertices.size()))
        .addVertexBuffer(
            std::move(vertexBuffer),
            0,
            Magnum::Shaders::FlatGL3D::Position{}
        );

    return mesh;
}

}