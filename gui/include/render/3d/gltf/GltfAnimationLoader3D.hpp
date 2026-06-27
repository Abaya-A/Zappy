#pragma once

#include "render/3d/Model3D.hpp"

#include <Magnum/Trade/AbstractImporter.h>

#include <vector>

namespace zappy::render3d {

class GltfAnimationLoader3D {
public:
    static std::vector<AnimationClip3D> loadAnimations(
        Magnum::Trade::AbstractImporter &importer
    );
};

}