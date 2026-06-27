#include "render/3d/ResourceModelRenderer3D.hpp"

#include "render/3d/ObjModelLoader3D.hpp"
#include "render/3d/PlanetGeometry3D.hpp"
#include "state/Resource.hpp"
#include "state/Tile.hpp"

#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>

#include <iostream>
#include <optional>

namespace zappy::render3d {
namespace {

constexpr float ModelSurfaceLift = 0.15f;
constexpr float FoodModelScale = 0.8f;

constexpr Magnum::Color4 FoodFallbackColor{0.95f, 0.75f, 0.25f, 1.0f};

const char *FoodModelPath = "assets/models/food.obj";

}

ResourceModelRenderer3D::ResourceModelRenderer3D(Magnum::Shaders::FlatGL3D &shader)
    : _modelRenderer(shader),
      _foodModel()
{
    loadModels();
}

void ResourceModelRenderer3D::loadModels()
{
    ObjModelLoader3D loader;
    const std::optional<Model3D> foodModel = loader.load(FoodModelPath);

    if (!foodModel.has_value()) {
        std::cerr << "[WARN]: failed to load model: " << FoodModelPath << std::endl;
        return;
    }

    _foodModel = foodModel.value();
    std::cout << "[INFO]: loaded model: " << FoodModelPath << std::endl;
}

void ResourceModelRenderer3D::draw(
    const GameState &state,
    const Magnum::Matrix4 &projection
)
{
    drawFoodModels(state, projection);
}

void ResourceModelRenderer3D::drawFoodModels(
    const GameState &state,
    const Magnum::Matrix4 &projection
)
{
    if (!_foodModel.isValid())
        return;

    for (int y = 0; y < state.height(); ++y) {
        for (int x = 0; x < state.width(); ++x)
            drawFoodOnTile(state, projection, x, y);
    }
}

void ResourceModelRenderer3D::drawFoodOnTile(
    const GameState &state,
    const Magnum::Matrix4 &projection,
    int tileX,
    int tileY
)
{
    const Tile *tile = state.tileAt(tileX, tileY);

    if (tile == nullptr)
        return;

    if (tile->quantity(ResourceType::Food) <= 0)
        return;

    _modelRenderer.draw(
        _foodModel,
        projection,
        foodTransform(tileX, tileY, state.width(), state.height()),
        FoodFallbackColor
    );
}

Magnum::Matrix4 ResourceModelRenderer3D::foodTransform(
    int tileX,
    int tileY,
    int mapWidth,
    int mapHeight
) const
{
    return PlanetGeometry3D::surfaceTransform(
        tileX,
        tileY,
        mapWidth,
        mapHeight,
        ModelSurfaceLift,
        FoodModelScale
    );
}

}