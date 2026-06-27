#pragma once

#include <Magnum/Math/Vector3.h>
#include <Magnum/Math/Vector4.h>

namespace zappy::render3d {

struct Vertex3D {
    Magnum::Vector3 position;
};

struct SkinnedVertex3D {
    Magnum::Vector3 position;
    Magnum::Vector4 jointIds;
    Magnum::Vector4 weights;
};

}