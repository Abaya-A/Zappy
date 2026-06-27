#pragma once

#include "render/3d/Model3D.hpp"

#include <Magnum/Trade/AbstractImporter.h>

#include <vector>

namespace zappy::render3d {

class GltfSkinLoader3D {
public:
    static std::vector<Skin3D> loadSkins(
        Magnum::Trade::AbstractImporter &importer
    );
};

}