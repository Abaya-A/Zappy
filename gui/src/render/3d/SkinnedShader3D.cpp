#include "render/3d/SkinnedShader3D.hpp"

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/Assert.h>

#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Functions.h>

#include <array>
#include <cstddef>

namespace zappy::render3d {
namespace {

const char *VertexShaderSource = R"GLSL(
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 jointIds;
layout(location = 2) in vec4 weights;

uniform mat4 transformationProjectionMatrix;
uniform mat4 meshMatrix;
uniform mat4 jointMatrices[64];

void main()
{
    mat4 skin =
        jointMatrices[int(jointIds.x)] * weights.x +
        jointMatrices[int(jointIds.y)] * weights.y +
        jointMatrices[int(jointIds.z)] * weights.z +
        jointMatrices[int(jointIds.w)] * weights.w;

    gl_Position =
        transformationProjectionMatrix *
        skin *
        meshMatrix *
        vec4(position, 1.0);
}
)GLSL";

const char *FragmentShaderSource = R"GLSL(
uniform vec4 color;

out vec4 fragmentColor;

void main()
{
    fragmentColor = color;
}
)GLSL";

}

SkinnedShader3D::SkinnedShader3D()
    : _transformationProjectionMatrixUniform{-1},
      _meshMatrixUniform{-1},
      _colorUniform{-1},
      _jointMatricesUniform{-1}
{
    Magnum::GL::Shader vertex{
        Magnum::GL::Version::GL330,
        Magnum::GL::Shader::Type::Vertex
    };

    Magnum::GL::Shader fragment{
        Magnum::GL::Version::GL330,
        Magnum::GL::Shader::Type::Fragment
    };

    vertex.addSource(VertexShaderSource);
    fragment.addSource(FragmentShaderSource);

    CORRADE_INTERNAL_ASSERT_OUTPUT(vertex.compile());
    CORRADE_INTERNAL_ASSERT_OUTPUT(fragment.compile());

    attachShaders({vertex, fragment});

    bindAttributeLocation(Position::Location, "position");
    bindAttributeLocation(JointIds::Location, "jointIds");
    bindAttributeLocation(Weights::Location, "weights");

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    _transformationProjectionMatrixUniform =
        uniformLocation("transformationProjectionMatrix");
    _meshMatrixUniform = uniformLocation("meshMatrix");
    _colorUniform = uniformLocation("color");
    _jointMatricesUniform = uniformLocation("jointMatrices");
}

SkinnedShader3D &SkinnedShader3D::setTransformationProjectionMatrix(
    const Magnum::Matrix4 &matrix
)
{
    setUniform(_transformationProjectionMatrixUniform, matrix);
    return *this;
}

SkinnedShader3D &SkinnedShader3D::setMeshMatrix(
    const Magnum::Matrix4 &matrix
)
{
    setUniform(_meshMatrixUniform, matrix);
    return *this;
}

SkinnedShader3D &SkinnedShader3D::setColor(
    const Magnum::Color4 &color
)
{
    setUniform(_colorUniform, color);
    return *this;
}

SkinnedShader3D &SkinnedShader3D::setJointMatrices(
    const std::vector<Magnum::Matrix4> &matrices
)
{
    std::array<Magnum::Matrix4, MaxJointMatrices> paddedMatrices;

    for (Magnum::Matrix4 &matrix : paddedMatrices)
        matrix = Magnum::Matrix4{Magnum::Math::IdentityInit};

    const std::size_t count = Magnum::Math::min<std::size_t>(
        matrices.size(),
        MaxJointMatrices
    );

    for (std::size_t i = 0; i < count; ++i)
        paddedMatrices[i] = matrices[i];

    setUniform(
        _jointMatricesUniform,
        Corrade::Containers::arrayView(
            paddedMatrices.data(),
            paddedMatrices.size()
        )
    );

    return *this;
}

}