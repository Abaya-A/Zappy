#include "render/3d/SkinnedModelRenderer3D.hpp"

#include <Corrade/Containers/ArrayView.h>

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>

#include <iostream>
#include <utility>

namespace zappy::render3d {
namespace {

Corrade::Containers::ArrayView<const char> vertexBytes(
    const std::vector<SkinnedVertex3D> &vertices
)
{
    return Corrade::Containers::arrayCast<const char>(
        Corrade::Containers::arrayView(
            vertices.data(),
            vertices.size()
        )
    );
}

}

SkinnedModelRenderer3D::SkinnedModelRenderer3D()
    : _shader(),
      _meshes(),
      _uploadedModel(nullptr)
{
    std::cout << "[SKINNED3D] renderer constructed" << std::endl;
}

bool SkinnedModelRenderer3D::shouldUpload(const Model3D &model) const
{
    return _uploadedModel != &model;
}

void SkinnedModelRenderer3D::uploadModel(const Model3D &model)
{
    std::cout << "[SKINNED3D] upload begin"
              << " skinnedSubMeshes=" << model.skinnedSubMeshes().size()
              << std::endl;

    _meshes.clear();

    for (std::size_t i = 0; i < model.skinnedSubMeshes().size(); ++i) {
        const SkinnedSubMesh3D &subMesh = model.skinnedSubMeshes()[i];

        std::cout << "[SKINNED3D] uploading submesh=" << i
                  << " vertices=" << subMesh.vertices.size()
                  << " vertexSize=" << sizeof(SkinnedVertex3D)
                  << std::endl;

        if (subMesh.vertices.empty())
            continue;

        UploadedSubMesh uploaded;
        uploaded.vertexCount = subMesh.vertices.size();
        uploaded.color = subMesh.color;

        uploaded.vertexBuffer.setData(
            vertexBytes(subMesh.vertices),
            Magnum::GL::BufferUsage::StaticDraw
        );

        std::cout << "[SKINNED3D] buffer uploaded submesh=" << i << std::endl;

        uploaded.mesh
            .setPrimitive(Magnum::GL::MeshPrimitive::Triangles)
            .setCount(static_cast<Magnum::Int>(uploaded.vertexCount))
            .addVertexBuffer(
                uploaded.vertexBuffer,
                0,
                SkinnedShader3D::Position{},
                SkinnedShader3D::JointIds{},
                SkinnedShader3D::Weights{}
            );

        std::cout << "[SKINNED3D] mesh configured submesh=" << i << std::endl;

        _meshes.push_back(std::move(uploaded));
    }

    _uploadedModel = &model;

    std::cout << "[SKINNED3D] uploaded skinned submeshes="
              << _meshes.size()
              << std::endl;
}

void SkinnedModelRenderer3D::draw(
    const Model3D &model,
    const Magnum::Matrix4 &projection,
    const Magnum::Matrix4 &transform,
    const std::vector<Magnum::Matrix4> &jointMatrices,
    const Magnum::Color4 &fallbackColor
)
{
    if (!model.isSkinned()) {
        std::cerr << "[SKINNED3D] draw skipped: model is not skinned"
                  << std::endl;
        return;
    }

    if (jointMatrices.empty()) {
        std::cerr << "[SKINNED3D] draw skipped: no joint matrices"
                  << std::endl;
        return;
    }

    if (shouldUpload(model))
        uploadModel(model);

    if (_meshes.empty()) {
        std::cerr << "[SKINNED3D] draw skipped: no uploaded meshes"
                  << std::endl;
        return;
    }

    const Magnum::Matrix4 transformationProjection = projection * transform;
    const Magnum::Matrix4 identityMeshMatrix{
        Magnum::Math::IdentityInit
    };

    for (std::size_t i = 0; i < _meshes.size(); ++i) {
        UploadedSubMesh &uploaded = _meshes[i];

        const Magnum::Color4 color =
            uploaded.color.a() > 0.0f ? uploaded.color : fallbackColor;

        _shader
            .setTransformationProjectionMatrix(transformationProjection)
            .setMeshMatrix(identityMeshMatrix)
            .setColor(color)
            .setJointMatrices(jointMatrices)
            .draw(uploaded.mesh);
    }
}

}