#include "render/3d/PlayerModelRenderer3D.hpp"

#include "render/3d/GltfModelLoader3D.hpp"
#include "render/3d/PlanetGeometry3D.hpp"

#include <iostream>

#include <Magnum/Math/Angle.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Vector3.h>

#include <iostream>
#include <optional>

namespace zappy::render3d {
namespace {

constexpr float PlayerSurfaceLift = 0.2f;
constexpr float PlayerScale = 25.0f;

constexpr Magnum::Color4 PlayerFallbackColor{0.75f, 0.85f, 1.0f, 1.0f};

const char *PlayerModelPath = "assets/models/Ghost.glb";

Magnum::Rad orientationAngle(int orientation)
{
    switch (orientation) {
        case 1:
            return Magnum::Deg{0.0f};
        case 2:
            return Magnum::Deg{-90.0f};
        case 3:
            return Magnum::Deg{180.0f};
        case 4:
            return Magnum::Deg{90.0f};
        default:
            return Magnum::Deg{0.0f};
    }
}

}

PlayerModelRenderer3D::PlayerModelRenderer3D(Magnum::Shaders::FlatGL3D &shader)
    : _modelRenderer(shader),
      _playerModel()
{
    std::cerr << "[PLAYER3D] renderer constructed" << std::endl;
    loadModel();
}

void PlayerModelRenderer3D::loadModel()
{
    GltfModelLoader3D loader;
    const std::optional<Model3D> model = loader.load(PlayerModelPath);

    if (!model.has_value()) {
        std::cerr << "[WARN]: failed to load model: " << PlayerModelPath << std::endl;
        return;
    }

    _playerModel = model.value();
    std::cout << "[INFO]: loaded model: " << PlayerModelPath << std::endl;
}

void PlayerModelRenderer3D::draw(
    const GameState &state,
    const Magnum::Matrix4 &projection
)
{
    std::cout << "[PLAYER3D] players=" << state.players().size() << std::endl;
    if (!_playerModel.isValid()) {
     //   std::cout << "[PLAYER3D] invalid model" << std::endl;
        return;
    }

    std::cout << "[PLAYER3D] players=" << state.players().size() << std::endl;

    for (const auto &[id, player] : state.players())
        drawPlayer(player, state.width(), state.height(), projection);
}

void PlayerModelRenderer3D::drawPlayer(
    const Player &player,
    int mapWidth,
    int mapHeight,
    const Magnum::Matrix4 &projection
)
{
    _modelRenderer.draw(
        _playerModel,
        projection,
        playerTransform(player, mapWidth, mapHeight),
        PlayerFallbackColor
    );
}

Magnum::Matrix4 PlayerModelRenderer3D::playerTransform(
    const Player &player,
    int mapWidth,
    int mapHeight
) const
{
    return PlanetGeometry3D::surfaceTransform(
        player.x(),
        player.y(),
        mapWidth,
        mapHeight,
        PlayerSurfaceLift,
        PlayerScale
    ) * orientationTransform(player.orientation());
}

Magnum::Matrix4 PlayerModelRenderer3D::orientationTransform(int orientation) const
{
    return Magnum::Matrix4::rotationY(orientationAngle(orientation));
}

}