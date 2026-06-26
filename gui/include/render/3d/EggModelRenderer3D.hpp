#pragma once

#include "render/3d/Model3D.hpp"
#include "render/3d/ModelRenderer3D.hpp"
#include "state/GameState.hpp"

#include <Magnum/Math/Matrix4.h>
#include <Magnum/Shaders/FlatGL.h>

namespace zappy::render3d {

class EggModelRenderer3D {
public:
    explicit EggModelRenderer3D(Magnum::Shaders::FlatGL3D &shader);

    void draw(const GameState &state, const Magnum::Matrix4 &projection);

private:
    void loadModel();

    void drawEgg(
        const Egg &egg,
        int mapWidth,
        int mapHeight,
        const Magnum::Matrix4 &projection
    );

    Magnum::Matrix4 eggTransform(
        const Egg &egg,
        int mapWidth,
        int mapHeight
    ) const;

    ModelRenderer3D _modelRenderer;
    Model3D _eggModel;
};

}