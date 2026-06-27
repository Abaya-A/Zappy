#pragma once

#include "render/3d/Model3D.hpp"
#include "render/3d/SkinnedShader3D.hpp"

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>

#include <cstddef>
#include <vector>

namespace zappy::render3d {

class SkinnedModelRenderer3D {
public:
    SkinnedModelRenderer3D();

    void draw(
        const Model3D &model,
        const Magnum::Matrix4 &projection,
        const Magnum::Matrix4 &transform,
        const std::vector<Magnum::Matrix4> &jointMatrices,
        const Magnum::Color4 &fallbackColor
    );

private:
    struct UploadedSubMesh {
        Magnum::GL::Buffer vertexBuffer;
        Magnum::GL::Mesh mesh;
        std::size_t vertexCount;
        Magnum::Color4 color;
    };

    void uploadModel(const Model3D &model);
    bool shouldUpload(const Model3D &model) const;

    SkinnedShader3D _shader;
    std::vector<UploadedSubMesh> _meshes;
    const Model3D *_uploadedModel;
};

}