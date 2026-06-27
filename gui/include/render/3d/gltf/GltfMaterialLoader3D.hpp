#pragma once

#include "render/3d/Model3D.hpp"

#include <Magnum/Magnum.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Trade/AbstractImporter.h>

#include <vector>

namespace zappy::render3d {

class GltfMaterialLoader3D {
public:
    static Magnum::Color4 debugMeshColor(Magnum::UnsignedInt meshIndex);

    static Magnum::Color4 materialColor(
        Magnum::Trade::AbstractImporter &importer,
        Magnum::Int materialId,
        Magnum::UnsignedInt fallbackMeshIndex
    );

    static std::vector<Magnum::Int> loadMeshMaterials(
        Magnum::Trade::AbstractImporter &importer
    );
};

}