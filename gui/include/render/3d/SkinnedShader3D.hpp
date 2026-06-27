#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Attribute.h>
#include <Magnum/Magnum.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/Math/Vector4.h>

#include <vector>

namespace zappy::render3d {

class SkinnedShader3D : public Magnum::GL::AbstractShaderProgram {
public:
    using Position = Magnum::GL::Attribute<0, Magnum::Vector3>;
    using JointIds = Magnum::GL::Attribute<1, Magnum::Vector4>;
    using Weights = Magnum::GL::Attribute<2, Magnum::Vector4>;

    static constexpr Magnum::UnsignedInt MaxJointMatrices = 64;

    SkinnedShader3D();

    SkinnedShader3D &setTransformationProjectionMatrix(
        const Magnum::Matrix4 &matrix
    );

    SkinnedShader3D &setMeshMatrix(
        const Magnum::Matrix4 &matrix
    );

    SkinnedShader3D &setColor(
        const Magnum::Color4 &color
    );

    SkinnedShader3D &setJointMatrices(
        const std::vector<Magnum::Matrix4> &matrices
    );

private:
    Magnum::Int _transformationProjectionMatrixUniform;
    Magnum::Int _meshMatrixUniform;
    Magnum::Int _colorUniform;
    Magnum::Int _jointMatricesUniform;
};

}