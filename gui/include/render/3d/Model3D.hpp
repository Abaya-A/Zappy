#pragma once

#include "render/3d/RenderGeometry3D.hpp"

#include <Magnum/Math/Color.h>

#include <vector>

namespace zappy::render3d {

struct ModelSubMesh3D {
    std::vector<Vertex3D> vertices;
    Magnum::Color4 color;
};

class Model3D {
public:
    Model3D();
    explicit Model3D(std::vector<Vertex3D> vertices);
    explicit Model3D(std::vector<ModelSubMesh3D> subMeshes);

    bool isValid() const;

    const std::vector<Vertex3D> &vertices() const;
    const std::vector<ModelSubMesh3D> &subMeshes() const;

private:
    std::vector<Vertex3D> _vertices;
    std::vector<ModelSubMesh3D> _subMeshes;
};

}