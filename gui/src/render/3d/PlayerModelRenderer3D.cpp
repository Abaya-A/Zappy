#include "render/3d/PlayerModelRenderer3D.hpp"

#include "render/3d/GltfModelLoader3D.hpp"
#include "render/3d/PlanetGeometry3D.hpp"
#include "render/3d/SkinnedAnimation3D.hpp"

#include <Magnum/Math/Angle.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Functions.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Vector3.h>

#include <chrono>
#include <iostream>
#include <optional>
#include <vector>

namespace zappy::render3d {
namespace {

constexpr float PlayerSurfaceLift = 0.15f;
constexpr float PlayerScale = 0.5f;
constexpr float PlayerMoveDuration = 0.6f;
constexpr float MaxFrameDelta = 0.1f;

constexpr Magnum::Color4 PlayerFallbackColor{0.75f, 0.85f, 1.0f, 1.0f};

constexpr const char *PlayerModelPath = "assets/models/Ghost.glb";

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

float smoothStep(float value)
{
    if (value <= 0.0f)
        return 0.0f;

    if (value >= 1.0f)
        return 1.0f;

    return value * value * (3.0f - 2.0f * value);
}

float lerpFloat(float from, float to, float factor)
{
    return from + (to - from) * factor;
}

}

PlayerModelRenderer3D::PlayerModelRenderer3D(Magnum::Shaders::FlatGL3D &shader)
    : _modelRenderer(shader),
      _skinnedRenderer(),
      _playerModel(),
      _visualPlayers(),
      _lastFrameTime(0.0f)
{
    std::cerr << "[PLAYER3D] renderer constructed" << std::endl;

    loadModel();
    logSkinningDebug();
}

void PlayerModelRenderer3D::loadModel()
{
    GltfModelLoader3D loader;
    const std::optional<Model3D> model = loader.load(PlayerModelPath);

    if (!model.has_value()) {
        std::cerr << "[WARN]: failed to load model: "
                  << PlayerModelPath
                  << std::endl;
        return;
    }

    _playerModel = model.value();

    std::cout << "[INFO]: loaded model: "
              << PlayerModelPath
              << std::endl;
}

void PlayerModelRenderer3D::logSkinningDebug() const
{
    const std::vector<Magnum::Matrix4> joints =
        SkinnedAnimation3D::jointMatrices(_playerModel, 0.5f);

    std::cout << "[PLAYER3D] model valid="
              << _playerModel.isValid()
              << " skinned="
              << _playerModel.isSkinned()
              << " nodes="
              << _playerModel.nodes().size()
              << " skins="
              << _playerModel.skins().size()
              << " animations="
              << _playerModel.animations().size()
              << " testJointMatrices="
              << joints.size()
              << std::endl;
}

float PlayerModelRenderer3D::animationTimeSeconds() const
{
    using Clock = std::chrono::steady_clock;

    static const Clock::time_point startTime = Clock::now();

    const std::chrono::duration<float> elapsed = Clock::now() - startTime;

    return elapsed.count();
}

float PlayerModelRenderer3D::frameDeltaSeconds()
{
    const float currentTime = animationTimeSeconds();

    if (_lastFrameTime <= 0.0f) {
        _lastFrameTime = currentTime;
        return 0.0f;
    }

    const float delta = currentTime - _lastFrameTime;
    _lastFrameTime = currentTime;

    if (delta < 0.0f)
        return 0.0f;

    if (delta > MaxFrameDelta)
        return MaxFrameDelta;

    return delta;
}

void PlayerModelRenderer3D::draw(
    const GameState &state,
    const Magnum::Matrix4 &projection
)
{
    if (!_playerModel.isValid())
        return;

    const float deltaSeconds = frameDeltaSeconds();

    for (const auto &[id, player] : state.players()) {
        drawPlayer(
            id,
            player,
            state.width(),
            state.height(),
            projection,
            deltaSeconds
        );
    }
}

void PlayerModelRenderer3D::drawPlayer(
    int playerId,
    const Player &player,
    int mapWidth,
    int mapHeight,
    const Magnum::Matrix4 &projection,
    float deltaSeconds
)
{
    VisualPlayerState &visual = visualStateFor(playerId, player);

    updateVisualState(visual, player, deltaSeconds);

    const Magnum::Matrix4 transform =
        playerTransform(player, visual, mapWidth, mapHeight);

    if (_playerModel.isSkinned()) {
        const std::vector<Magnum::Matrix4> joints =
            SkinnedAnimation3D::jointMatrices(
                _playerModel,
                animationTimeSeconds()
            );

        if (!joints.empty()) {
            _skinnedRenderer.draw(
                _playerModel,
                projection,
                transform,
                joints,
                PlayerFallbackColor
            );
            return;
        }
    }

    _modelRenderer.draw(
        _playerModel,
        projection,
        transform,
        PlayerFallbackColor
    );
}

PlayerModelRenderer3D::VisualPlayerState &PlayerModelRenderer3D::visualStateFor(
    int playerId,
    const Player &player
)
{
    VisualPlayerState &visual = _visualPlayers[playerId];

    if (!visual.initialized) {
        visual.currentX = static_cast<float>(player.x());
        visual.currentY = static_cast<float>(player.y());

        visual.startX = visual.currentX;
        visual.startY = visual.currentY;

        visual.targetX = visual.currentX;
        visual.targetY = visual.currentY;

        visual.elapsed = 0.0f;
        visual.duration = PlayerMoveDuration;

        visual.initialized = true;
    }

    return visual;
}

void PlayerModelRenderer3D::updateVisualState(
    VisualPlayerState &visual,
    const Player &player,
    float deltaSeconds
)
{
    const float targetX = static_cast<float>(player.x());
    const float targetY = static_cast<float>(player.y());

    if (visual.targetX != targetX || visual.targetY != targetY) {
        visual.startX = visual.currentX;
        visual.startY = visual.currentY;

        visual.targetX = targetX;
        visual.targetY = targetY;

        visual.elapsed = 0.0f;
        visual.duration = PlayerMoveDuration;
    }

    if (visual.elapsed >= visual.duration) {
        visual.currentX = visual.targetX;
        visual.currentY = visual.targetY;
        return;
    }

    visual.elapsed += deltaSeconds;

    const float progress = visual.duration > 0.0f
        ? visual.elapsed / visual.duration
        : 1.0f;

    const float factor = smoothStep(progress);

    visual.currentX = lerpFloat(visual.startX, visual.targetX, factor);
    visual.currentY = lerpFloat(visual.startY, visual.targetY, factor);
}

Magnum::Matrix4 PlayerModelRenderer3D::playerTransform(
    const Player &player,
    const VisualPlayerState &visual,
    int mapWidth,
    int mapHeight
) const
{
    return playerTransformAt(
        visual.currentX,
        visual.currentY,
        player.orientation(),
        mapWidth,
        mapHeight
    );
}

Magnum::Matrix4 PlayerModelRenderer3D::playerTransformAt(
    float x,
    float y,
    int orientation,
    int mapWidth,
    int mapHeight
) const
{
    const Magnum::Matrix4 planetTransform =
        PlanetGeometry3D::surfaceTransform(
            x,
            y,
            mapWidth,
            mapHeight,
            PlayerSurfaceLift,
            PlayerScale
        );

    const Magnum::Matrix4 localOffset =
        Magnum::Matrix4::translation(Magnum::Vector3{0.0f, 0.35f, 0.0f});

    return planetTransform
        * orientationTransform(orientation)
        * localOffset;
}

Magnum::Matrix4 PlayerModelRenderer3D::orientationTransform(int orientation) const
{
    return Magnum::Matrix4::rotationY(orientationAngle(orientation));
}

}