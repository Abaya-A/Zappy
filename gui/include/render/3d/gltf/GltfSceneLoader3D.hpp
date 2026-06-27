#pragma once

#include "render/3d/Model3D.hpp"

#include <Magnum/Trade/AbstractImporter.h>

#include <vector>

namespace zappy::render3d {

class GltfSceneLoader3D {
public:
    static std::vector<SceneNode3D> loadSceneNodes(
        Magnum::Trade::AbstractImporter &importer
    );
};

}