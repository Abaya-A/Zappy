#pragma once

#include "state/GameState.hpp"

#include <Magnum/Math/Matrix4.h>
#include <Magnum/Shaders/FlatGL.h>

namespace zappy::render3d {

class MapRenderer3D {
public:
    explicit MapRenderer3D(Magnum::Shaders::FlatGL3D &shader);

    void draw(const GameState &state, const Magnum::Matrix4 &projection);

private:
    Magnum::Shaders::FlatGL3D &_shader;
};

}