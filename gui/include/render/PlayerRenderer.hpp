#pragma once

#include "state/GameState.hpp"

#include <Magnum/Math/Matrix3.h>
#include <Magnum/Shaders/FlatGL.h>

namespace zappy::render {

class PlayerRenderer {
public:
    explicit PlayerRenderer(Magnum::Shaders::FlatGL2D &shader);

    void draw(const GameState &state, const Magnum::Matrix3 &projection);

private:
    Magnum::Shaders::FlatGL2D &_shader;
};

}