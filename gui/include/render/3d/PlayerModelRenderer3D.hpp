#pragma once

#include "render/3d/Model3D.hpp"
#include "render/3d/ModelRenderer3D.hpp"
#include "state/GameState.hpp"
#include "state/Player.hpp"

#include <Magnum/Math/Matrix4.h>
#include <Magnum/Shaders/FlatGL.h>

namespace zappy::render3d {

class PlayerModelRenderer3D {
public:
    explicit PlayerModelRenderer3D(Magnum::Shaders::FlatGL3D &shader);

    void draw(const GameState &state, const Magnum::Matrix4 &projection);

private:
    void loadModel();

    void drawPlayer(
        const Player &player,
        int mapWidth,
        int mapHeight,
        const Magnum::Matrix4 &projection
    );

    Magnum::Matrix4 playerTransform(
        const Player &player,
        int mapWidth,
        int mapHeight
    ) const;

    Magnum::Matrix4 orientationTransform(int orientation) const;

    ModelRenderer3D _modelRenderer;
    Model3D _playerModel;
};

}