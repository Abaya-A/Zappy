#pragma once

#include "render/3d/Model3D.hpp"

#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Quaternion.h>
#include <Magnum/Math/Vector3.h>

#include <vector>

namespace zappy::render3d {

class SkinnedAnimation3D {
public:
    static std::vector<Magnum::Matrix4> jointMatrices(
        const Model3D &model,
        float timeSeconds
    );

private:
    static Magnum::Vector3 sampleTranslation(
        const AnimationTrack3D &track,
        float timeSeconds
    );

    static Magnum::Quaternion sampleRotation(
        const AnimationTrack3D &track,
        float timeSeconds
    );

    static Magnum::Vector3 sampleScaling(
        const AnimationTrack3D &track,
        float timeSeconds
    );

    static std::vector<Magnum::Matrix4> animatedLocalTransforms(
        const Model3D &model,
        const AnimationClip3D &clip,
        float timeSeconds
    );

    static std::vector<Magnum::Matrix4> globalTransforms(
        const std::vector<SceneNode3D> &nodes,
        const std::vector<Magnum::Matrix4> &locals
    );
};

}