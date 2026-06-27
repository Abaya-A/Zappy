#pragma once

#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Shaders/FlatGL.h>

namespace zappy::render {

class HudIconRenderer {
public:
    explicit HudIconRenderer(Magnum::Shaders::FlatGL2D &shader);

    void drawPlayerIcon(
        const Magnum::Matrix3 &projection,
        const Magnum::Vector2 &position,
        float scale,
        const Magnum::Color4 &color
    );

    void drawEggIcon(
        const Magnum::Matrix3 &projection,
        const Magnum::Vector2 &position,
        float scale,
        const Magnum::Color4 &color
    );

private:
    Magnum::Shaders::FlatGL2D &_shader;
};

}