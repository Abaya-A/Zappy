#pragma once

#include "render/3d/Model3D.hpp"
#include "render/3d/ModelRenderer3D.hpp"
#include "render/3d/SkinnedModelRenderer3D.hpp"
#include "state/GameState.hpp"
#include "state/Player.hpp"

#include <Magnum/Math/Matrix4.h>
#include <Magnum/Shaders/FlatGL.h>

#include <unordered_map>

namespace zappy::render3d {

class PlayerModelRenderer3D {
public:
    explicit PlayerModelRenderer3D(Magnum::Shaders::FlatGL3D &shader);

    void draw(
        const GameState &state,
        const Magnum::Matrix4 &projection
    );

private:
    struct VisualPlayerState {
        float currentX;
        float currentY;

        float startX;
        float startY;

        float targetX;
        float targetY;

        float elapsed;
        float duration;

        bool initialized;
    };

    void loadModel();
    void logSkinningDebug() const;

    void drawPlayer(
        int playerId,
        const Player &player,
        int mapWidth,
        int mapHeight,
        const Magnum::Matrix4 &projection,
        float deltaSeconds
    );

    VisualPlayerState &visualStateFor(
        int playerId,
        const Player &player
    );

    void updateVisualState(
        VisualPlayerState &visual,
        const Player &player,
        float deltaSeconds
    );

    Magnum::Matrix4 playerTransform(
        const Player &player,
        const VisualPlayerState &visual,
        int mapWidth,
        int mapHeight
    ) const;

    Magnum::Matrix4 playerTransformAt(
        float x,
        float y,
        int orientation,
        int mapWidth,
        int mapHeight
    ) const;

    Magnum::Matrix4 orientationTransform(int orientation) const;

    float animationTimeSeconds() const;
    float frameDeltaSeconds();

    ModelRenderer3D _modelRenderer;
    SkinnedModelRenderer3D _skinnedRenderer;
    Model3D _playerModel;

    std::unordered_map<int, VisualPlayerState> _visualPlayers;
    float _lastFrameTime;
};

}