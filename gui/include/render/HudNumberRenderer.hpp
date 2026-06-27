#pragma once

#include "render/RenderGeometry.hpp"

#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Shaders/FlatGL.h>

namespace zappy::render {

class HudNumberRenderer {
public:
    explicit HudNumberRenderer(Magnum::Shaders::FlatGL2D &shader);

    void drawNumber(
        const Magnum::Matrix3 &projection,
        const Magnum::Vector2 &position,
        int value,
        float scale,
        const Magnum::Color4 &color
    );

private:
    void drawDigit(
        const Magnum::Matrix3 &projection,
        const Magnum::Vector2 &position,
        int digit,
        float scale,
        const Magnum::Color4 &color
    );

    Magnum::Shaders::FlatGL2D &_shader;
};

}