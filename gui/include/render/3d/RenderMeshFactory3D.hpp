#pragma once

#include "render/3d/RenderGeometry3D.hpp"

#include <Magnum/GL/Mesh.h>
#include <Magnum/MeshTools/Compile.h>

#include <vector>

namespace zappy::render3d {

enum class MeshPrimitive3D {
    Lines,
    Triangles
};

class RenderMeshFactory3D {
public:
    static Magnum::GL::Mesh build(
        MeshPrimitive3D primitive,
        const std::vector<Vertex3D> &vertices
    );
};

}