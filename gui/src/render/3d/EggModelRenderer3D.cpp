#include "render/3d/EggModelRenderer3D.hpp"

#include "render/3d/ObjModelLoader3D.hpp"
#include "render/3d/PlanetGeometry3D.hpp"
#include "state/Egg.hpp"

#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>

#include <iostream>
#include <optional>

namespace zappy::render3d {
namespace {

constexpr float EggSurfaceLift = 0.24f;
constexpr float EggScale = 1.0f;

constexpr Magnum::Color4 EggFallbackColor{0.95f, 0.92f, 0.82f, 1.0f};

const char *EggModelPath = "assets/models/CHAHIN_UNCRACKED_EGG.obj";

}

EggModelRenderer3D::EggModelRenderer3D(Magnum::Shaders::FlatGL3D &shader)
    : _modelRenderer(shader),
      _eggModel()
{
    loadModel();
}

void EggModelRenderer3D::loadModel()
{
    ObjModelLoader3D loader;
    const std::optional<Model3D> model = loader.load(EggModelPath);

    if (!model.has_value()) {
        std::cerr << "[WARN]: failed to load model: " << EggModelPath << std::endl;
        return;
    }

    _eggModel = model.value();
    std::cout << "[INFO]: loaded model: " << EggModelPath << std::endl;
}

void EggModelRenderer3D::draw(
    const GameState &state,
    const Magnum::Matrix4 &projection
)
{
    if (!_eggModel.isValid())
        return;

    for (const auto &[id, egg] : state.eggs())
        drawEgg(egg, state.width(), state.height(), projection);
}

void EggModelRenderer3D::drawEgg(
    const Egg &egg,
    int mapWidth,
    int mapHeight,
    const Magnum::Matrix4 &projection
)
{
    _modelRenderer.draw(
        _eggModel,
        projection,
        eggTransform(egg, mapWidth, mapHeight),
        EggFallbackColor
    );
}

Magnum::Matrix4 EggModelRenderer3D::eggTransform(
    const Egg &egg,
    int mapWidth,
    int mapHeight
) const
{
    return PlanetGeometry3D::surfaceTransform(
        egg.x(),
        egg.y(),
        mapWidth,
        mapHeight,
        EggSurfaceLift,
        EggScale
    );
}

}