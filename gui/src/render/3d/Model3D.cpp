#include "render/3d/Model3D.hpp"

#include <utility>

namespace zappy::render3d {

Model3D::Model3D()
    : _vertices(),
      _subMeshes()
{
}

Model3D::Model3D(std::vector<Vertex3D> vertices)
    : _vertices(std::move(vertices)),
      _subMeshes()
{
}

Model3D::Model3D(std::vector<ModelSubMesh3D> subMeshes)
    : _vertices(),
      _subMeshes(std::move(subMeshes))
{
}

bool Model3D::isValid() const
{
    if (!_vertices.empty())
        return true;

    for (const ModelSubMesh3D &subMesh : _subMeshes) {
        if (!subMesh.vertices.empty())
            return true;
    }

    return false;
}

const std::vector<Vertex3D> &Model3D::vertices() const
{
    return _vertices;
}

const std::vector<ModelSubMesh3D> &Model3D::subMeshes() const
{
    return _subMeshes;
}

}