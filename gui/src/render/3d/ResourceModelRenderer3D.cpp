#include "render/3d/ResourceModelRenderer3D.hpp"

#include "render/3d/GltfModelLoader3D.hpp"
#include "render/3d/ObjModelLoader3D.hpp"
#include "render/3d/PlanetGeometry3D.hpp"
#include "state/Resource.hpp"
#include "state/Tile.hpp"

#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>

#include <array>
#include <iostream>
#include <optional>

namespace zappy::render3d {
namespace {

constexpr float FoodSurfaceLift = 0.15f;
constexpr float StoneSurfaceLift = 0.22f;

constexpr float FoodModelScale = 0.8f;
constexpr float StoneModelScale = 5.0f;

constexpr Magnum::Color4 FoodFallbackColor{0.95f, 0.75f, 0.25f, 1.0f};

const char *FoodModelPath = "assets/models/food.obj";
const char *CrystalModelPath = "assets/models/Crystal.glb";

constexpr std::array<ResourceType, 6> StoneTypes = {
    ResourceType::Linemate,
    ResourceType::Deraumere,
    ResourceType::Sibur,
    ResourceType::Mendiane,
    ResourceType::Phiras,
    ResourceType::Thystame
};

Magnum::Color4 stoneColor(ResourceType type)
{
    switch (type) {
        case ResourceType::Linemate:
            return Magnum::Color4{0.20f, 0.75f, 1.00f, 1.0f};
        case ResourceType::Deraumere:
            return Magnum::Color4{0.85f, 0.45f, 1.00f, 1.0f};
        case ResourceType::Sibur:
            return Magnum::Color4{1.00f, 0.80f, 0.25f, 1.0f};
        case ResourceType::Mendiane:
            return Magnum::Color4{1.00f, 0.35f, 0.35f, 1.0f};
        case ResourceType::Phiras:
            return Magnum::Color4{0.35f, 1.00f, 0.80f, 1.0f};
        case ResourceType::Thystame:
            return Magnum::Color4{1.00f, 1.00f, 1.00f, 1.0f};
        default:
            return Magnum::Color4{0.80f, 0.80f, 0.80f, 1.0f};
    }
}

Magnum::Vector3 stoneLocalOffset(ResourceType type)
{
    switch (type) {
        case ResourceType::Linemate:
            return Magnum::Vector3{-0.55f, 0.0f, -0.35f};
        case ResourceType::Deraumere:
            return Magnum::Vector3{0.55f, 0.0f, -0.35f};
        case ResourceType::Sibur:
            return Magnum::Vector3{-0.55f, 0.0f, 0.35f};
        case ResourceType::Mendiane:
            return Magnum::Vector3{0.55f, 0.0f, 0.35f};
        case ResourceType::Phiras:
            return Magnum::Vector3{0.0f, 0.0f, -0.65f};
        case ResourceType::Thystame:
            return Magnum::Vector3{0.0f, 0.0f, 0.65f};
        default:
            return Magnum::Vector3{0.0f, 0.0f, 0.0f};
    }
}

float stoneAngleOffset(ResourceType type)
{
    return static_cast<float>(Resource::index(type)) * 0.55f;
}

float stoneTileOffset(ResourceType type)
{
    return static_cast<float>(Resource::index(type) - 1) * 0.04f;
}

}

ResourceModelRenderer3D::ResourceModelRenderer3D(Magnum::Shaders::FlatGL3D &shader)
    : _modelRenderer(shader),
      _foodModel(),
      _crystalModel()
{
    loadModels();
}

void ResourceModelRenderer3D::loadModels()
{
    ObjModelLoader3D objLoader;
    GltfModelLoader3D gltfLoader;

    const std::optional<Model3D> foodModel = objLoader.load(FoodModelPath);

    if (!foodModel.has_value())
        std::cerr << "[WARN]: failed to load model: " << FoodModelPath << std::endl;
    else {
        _foodModel = foodModel.value();
        std::cout << "[INFO]: loaded model: " << FoodModelPath << std::endl;
    }

    const std::optional<Model3D> crystalModel = gltfLoader.load(CrystalModelPath);

    if (!crystalModel.has_value())
        std::cerr << "[WARN]: failed to load model: " << CrystalModelPath << std::endl;
    else {
        _crystalModel = crystalModel.value();
        std::cout << "[INFO]: loaded model: " << CrystalModelPath << std::endl;
    }
}

void ResourceModelRenderer3D::draw(
    const GameState &state,
    const Magnum::Matrix4 &projection
)
{
    drawFoodModels(state, projection);
    drawStoneModels(state, projection);
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

void ResourceModelRenderer3D::drawStoneModels(
    const GameState &state,
    const Magnum::Matrix4 &projection
)
{
    if (!_crystalModel.isValid())
        return;

    for (int y = 0; y < state.height(); ++y) {
        for (int x = 0; x < state.width(); ++x) {
            for (ResourceType type : StoneTypes)
                drawStoneOnTile(state, projection, x, y, type);
        }
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

void ResourceModelRenderer3D::drawStoneOnTile(
    const GameState &state,
    const Magnum::Matrix4 &projection,
    int tileX,
    int tileY,
    ResourceType type
)
{
    const Tile *tile = state.tileAt(tileX, tileY);

    if (tile == nullptr)
        return;

    if (tile->quantity(type) <= 0)
        return;

    _modelRenderer.draw(
        _crystalModel,
        projection,
        stoneTransform(tileX, tileY, state.width(), state.height(), type),
        stoneColor(type)
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
        FoodSurfaceLift,
        FoodModelScale
    ) * Magnum::Matrix4::translation(
        Magnum::Vector3{0.0f, 0.0f, 0.30f} / FoodModelScale
    );
}

Magnum::Matrix4 ResourceModelRenderer3D::stoneTransform(
    int tileX,
    int tileY,
    int mapWidth,
    int mapHeight,
    ResourceType type
) const
{
    const Magnum::Vector3 localOffset = stoneLocalOffset(type);

    return PlanetGeometry3D::surfaceTransform(
        tileX,
        tileY,
        mapWidth,
        mapHeight,
        StoneSurfaceLift + stoneTileOffset(type),
        StoneModelScale
    )
    * Magnum::Matrix4::translation(localOffset / StoneModelScale)
    * Magnum::Matrix4::rotationY(Magnum::Rad{stoneAngleOffset(type)});
}

}