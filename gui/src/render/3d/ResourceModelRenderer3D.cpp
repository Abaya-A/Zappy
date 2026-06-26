#include "render/3d/ResourceModelRenderer3D.hpp"

#include "render/3d/ObjModelLoader3D.hpp"
#include "state/Resource.hpp"
#include "state/Tile.hpp"

#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Quaternion.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <optional>

namespace zappy::render3d {
namespace {

constexpr float Pi = 3.14159265358979323846f;

constexpr float PlanetRadiusScale = 0.45f;
constexpr float PlanetRadiusPadding = 2.0f;

constexpr float ModelSurfaceLift = 0.20f;
constexpr float FoodModelScale = 1.0f;

const char *FoodModelPath = "assets/models/food.obj";

float planetRadius(int width, int height)
{
    const float mapSize = static_cast<float>(std::max(width, height));
    return mapSize * PlanetRadiusScale + PlanetRadiusPadding;
}

float tileLongitude(int tileX, int mapWidth)
{
    return ((static_cast<float>(tileX) + 0.5f) / static_cast<float>(mapWidth))
        * 2.0f * Pi;
}

float tileLatitude(int tileY, int mapHeight)
{
    return -Pi * 0.5f
        + ((static_cast<float>(tileY) + 0.5f) / static_cast<float>(mapHeight))
        * Pi;
}

Magnum::Vector3 spherePoint(float radius, float longitude, float latitude)
{
    const float cosLatitude = std::cos(latitude);

    return Magnum::Vector3{
        radius * cosLatitude * std::cos(longitude),
        radius * std::sin(latitude),
        radius * cosLatitude * std::sin(longitude)
    };
}

Magnum::Vector3 tileCenterOnPlanet(
    int tileX,
    int tileY,
    int mapWidth,
    int mapHeight,
    float radius
)
{
    return spherePoint(
        radius,
        tileLongitude(tileX, mapWidth),
        tileLatitude(tileY, mapHeight)
    );
}

Magnum::Matrix4 alignModelUpToPlanetNormal(const Magnum::Vector3 &position)
{
    const Magnum::Vector3 normal = position.normalized();

    const Magnum::Quaternion alignment =
        Magnum::Quaternion::rotation(
            Magnum::Vector3::yAxis(),
            normal
        );

    return Magnum::Matrix4::from(
        alignment.toMatrix(),
        position
    );
}

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
        for (int x = 0; x < state.width(); ++x) {
            const Tile *tile = state.tileAt(x, y);

            if (tile == nullptr)
                continue;

            if (tile->quantity(ResourceType::Food) <= 0)
                continue;

            _modelRenderer.draw(
                _foodModel,
                projection,
                foodTransform(x, y, state.width(), state.height()),
                Magnum::Color4{0.95f, 0.75f, 0.25f, 1.0f}
            );
        }
    }
}

Magnum::Matrix4 ResourceModelRenderer3D::foodTransform(
    int tileX,
    int tileY,
    int mapWidth,
    int mapHeight
) const
{
    const float radius = planetRadius(mapWidth, mapHeight) + ModelSurfaceLift;

    const Magnum::Vector3 position = tileCenterOnPlanet(
        tileX,
        tileY,
        mapWidth,
        mapHeight,
        radius
    );

    return alignModelUpToPlanetNormal(position) *
        Magnum::Matrix4::scaling(Magnum::Vector3{FoodModelScale});
}

}