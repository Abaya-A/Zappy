#pragma once

#include "render/RenderGeometry.hpp"

#include <Magnum/GL/Mesh.h>

#include <vector>

namespace zappy::render {

class RenderMeshFactory {
public:
    static Magnum::GL::Mesh build(
        Magnum::GL::MeshPrimitive primitive,
        const std::vector<Vertex> &vertices
    );
};

}