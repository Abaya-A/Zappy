#include "render/3d/ModelRenderer3D.hpp"

#include "render/3d/RenderMeshFactory3D.hpp"

#include <Magnum/GL/Mesh.h>

namespace zappy::render3d {

ModelRenderer3D::ModelRenderer3D(Magnum::Shaders::FlatGL3D &shader)
    : _shader(shader)
{
}

void ModelRenderer3D::draw(
    const Model3D &model,
    const Magnum::Matrix4 &projection,
    const Magnum::Matrix4 &transform,
    const Magnum::Color4 &fallbackColor
)
{
    if (!model.isValid())
        return;

    if (!model.subMeshes().empty()) {
        for (const ModelSubMesh3D &subMesh : model.subMeshes()) {
            if (subMesh.vertices.empty())
                continue;

            Magnum::GL::Mesh mesh = RenderMeshFactory3D::build(
                MeshPrimitive3D::Triangles,
                subMesh.vertices
            );

            _shader
                .setTransformationProjectionMatrix(projection * transform)
                .setColor(fallbackColor)
                .draw(mesh);
        }
        return;
    }

    Magnum::GL::Mesh mesh = RenderMeshFactory3D::build(
        MeshPrimitive3D::Triangles,
        model.vertices()
    );

    _shader
        .setTransformationProjectionMatrix(projection * transform)
        .setColor(fallbackColor)
        .draw(mesh);
}

}