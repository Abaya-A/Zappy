#pragma once

#include "render/3d/RenderGeometry3D.hpp"
#include "state/GameState.hpp"

#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Shaders/FlatGL.h>

#include <optional>

namespace zappy::render3d {

class SelectedTileRenderer3D {
public:
    explicit SelectedTileRenderer3D(Magnum::Shaders::FlatGL3D &shader);

    void draw(
        const GameState &state,
        const Magnum::Matrix4 &projection,
        const std::optional<Magnum::Vector2i> &selectedTile
    );

private:
    Magnum::Shaders::FlatGL3D &_shader;
};

}