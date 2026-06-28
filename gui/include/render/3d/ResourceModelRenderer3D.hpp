#pragma once

#include "render/3d/Model3D.hpp"
#include "render/3d/ModelRenderer3D.hpp"
#include "state/GameState.hpp"
#include "state/Resource.hpp"

#include <Magnum/Math/Matrix4.h>
#include <Magnum/Shaders/FlatGL.h>

namespace zappy::render3d {

class ResourceModelRenderer3D {
public:
    explicit ResourceModelRenderer3D(Magnum::Shaders::FlatGL3D &shader);

    void draw(
        const GameState &state,
        const Magnum::Matrix4 &projection
    );

private:
    void loadModels();

    void drawFoodModels(
        const GameState &state,
        const Magnum::Matrix4 &projection
    );

    void drawStoneModels(
        const GameState &state,
        const Magnum::Matrix4 &projection
    );

    void drawFoodOnTile(
        const GameState &state,
        const Magnum::Matrix4 &projection,
        int tileX,
        int tileY
    );

    void drawStoneOnTile(
        const GameState &state,
        const Magnum::Matrix4 &projection,
        int tileX,
        int tileY,
        ResourceType type
    );

    Magnum::Matrix4 foodTransform(
        int tileX,
        int tileY,
        int mapWidth,
        int mapHeight
    ) const;

    Magnum::Matrix4 stoneTransform(
        int tileX,
        int tileY,
        int mapWidth,
        int mapHeight,
        ResourceType type
    ) const;

    ModelRenderer3D _modelRenderer;
    Model3D _foodModel;
    Model3D _crystalModel;
};

}