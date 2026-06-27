#pragma once

#include "render/3d/Model3D.hpp"

#include <Magnum/Magnum.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/MeshData.h>

#include <vector>

namespace zappy::render3d {

class GltfMeshLoader3D {
public:
    static std::vector<ModelSubMesh3D> loadSubMeshes(
        Magnum::Trade::AbstractImporter &importer,
        const std::vector<Magnum::Int> &meshMaterials
    );

    static std::vector<SkinnedSubMesh3D> loadSkinnedSubMeshes(
        Magnum::Trade::AbstractImporter &importer,
        const std::vector<Magnum::Int> &meshMaterials
    );

private:
    static std::vector<Vertex3D> meshVertices(
        const Magnum::Trade::MeshData &mesh
    );

    static bool hasSkinningAttributes(
        const Magnum::Trade::MeshData &mesh
    );

    static void logMeshAttributes(
        const Magnum::Trade::MeshData &mesh,
        Magnum::UnsignedInt meshIndex
    );

    static std::vector<SkinnedVertex3D> skinnedMeshVertices(
        const Magnum::Trade::MeshData &mesh
    );
};

}