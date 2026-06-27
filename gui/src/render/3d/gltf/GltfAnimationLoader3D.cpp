#include "render/3d/gltf/GltfAnimationLoader3D.hpp"

#include <Corrade/Containers/Optional.h>

#include <Magnum/Math/Quaternion.h>
#include <Magnum/Math/Range.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/AnimationData.h>

#include <cstddef>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace zappy::render3d {
namespace {

constexpr const char *PlayerIdleAnimationName =
    "CharacterArmature|Flying_Idle";

std::optional<Magnum::UnsignedInt> findAnimationByName(
    Magnum::Trade::AbstractImporter &importer,
    const std::string &name
)
{
    for (Magnum::UnsignedInt i = 0; i < importer.animationCount(); ++i) {
        const std::string animationName = importer.animationName(i);

        const Corrade::Containers::Optional<Magnum::Trade::AnimationData>
            animation = importer.animation(i);

        if (animation) {
            const Magnum::Range1D duration = animation->duration();

            std::cout << "[GLTF] animation " << i
                      << " name=\"" << animationName << "\""
                      << " tracks=" << animation->trackCount()
                      << " duration=[" << duration.min()
                      << ", " << duration.max() << "]"
                      << std::endl;
        }

        if (animationName == name) {
            std::cout << "[GLTF] selected idle animation \""
                      << animationName
                      << "\" index=" << i
                      << std::endl;
            return i;
        }
    }

    return std::nullopt;
}

AnimationTrackTarget3D toModelTrackTarget(
    Magnum::Trade::AnimationTrackTarget target
)
{
    if (target == Magnum::Trade::AnimationTrackTarget::Translation3D)
        return AnimationTrackTarget3D::Translation;

    if (target == Magnum::Trade::AnimationTrackTarget::Rotation3D)
        return AnimationTrackTarget3D::Rotation;

    return AnimationTrackTarget3D::Scaling;
}

bool isSupportedAnimationTrack(
    Magnum::Trade::AnimationTrackTarget target
)
{
    return target == Magnum::Trade::AnimationTrackTarget::Translation3D
        || target == Magnum::Trade::AnimationTrackTarget::Rotation3D
        || target == Magnum::Trade::AnimationTrackTarget::Scaling3D;
}

void appendTranslationKeys(
    const Magnum::Trade::AnimationData &animation,
    Magnum::UnsignedInt trackId,
    AnimationTrack3D &track
)
{
    const auto animationTrack =
        animation.track<Magnum::Vector3>(trackId);

    const auto keys = animationTrack.keys();
    const auto values = animationTrack.values();

    track.translations.reserve(animationTrack.size());

    for (std::size_t i = 0; i < animationTrack.size(); ++i) {
        track.translations.push_back(AnimationKeyTranslation3D{
            .time = keys[i],
            .value = values[i]
        });
    }
}

void appendRotationKeys(
    const Magnum::Trade::AnimationData &animation,
    Magnum::UnsignedInt trackId,
    AnimationTrack3D &track
)
{
    const auto animationTrack =
        animation.track<Magnum::Quaternion>(trackId);

    const auto keys = animationTrack.keys();
    const auto values = animationTrack.values();

    track.rotations.reserve(animationTrack.size());

    for (std::size_t i = 0; i < animationTrack.size(); ++i) {
        track.rotations.push_back(AnimationKeyRotation3D{
            .time = keys[i],
            .value = values[i]
        });
    }
}

void appendScalingKeys(
    const Magnum::Trade::AnimationData &animation,
    Magnum::UnsignedInt trackId,
    AnimationTrack3D &track
)
{
    const auto animationTrack =
        animation.track<Magnum::Vector3>(trackId);

    const auto keys = animationTrack.keys();
    const auto values = animationTrack.values();

    track.scalings.reserve(animationTrack.size());

    for (std::size_t i = 0; i < animationTrack.size(); ++i) {
        track.scalings.push_back(AnimationKeyScaling3D{
            .time = keys[i],
            .value = values[i]
        });
    }
}

bool fillAnimationTrack(
    const Magnum::Trade::AnimationData &animation,
    Magnum::UnsignedInt trackId,
    AnimationTrack3D &track
)
{
    const Magnum::Trade::AnimationTrackTarget target =
        animation.trackTargetName(trackId);

    if (!isSupportedAnimationTrack(target))
        return false;

    track.objectId = static_cast<unsigned int>(
        animation.trackTarget(trackId)
    );
    track.target = toModelTrackTarget(target);

    if (target == Magnum::Trade::AnimationTrackTarget::Translation3D) {
        appendTranslationKeys(animation, trackId, track);
        return !track.translations.empty();
    }

    if (target == Magnum::Trade::AnimationTrackTarget::Rotation3D) {
        appendRotationKeys(animation, trackId, track);
        return !track.rotations.empty();
    }

    if (target == Magnum::Trade::AnimationTrackTarget::Scaling3D) {
        appendScalingKeys(animation, trackId, track);
        return !track.scalings.empty();
    }

    return false;
}

AnimationClip3D convertAnimationClip(
    Magnum::Trade::AbstractImporter &importer,
    Magnum::UnsignedInt animationId,
    const Magnum::Trade::AnimationData &animation
)
{
    const Magnum::Range1D duration = animation.duration();

    AnimationClip3D clip;
    clip.name = importer.animationName(animationId);
    clip.duration = duration.max();

    clip.tracks.reserve(animation.trackCount());

    for (Magnum::UnsignedInt i = 0; i < animation.trackCount(); ++i) {
        AnimationTrack3D track;

        if (!fillAnimationTrack(animation, i, track)) {
            std::cout << "[GLTF] skipped animation track " << i
                      << " target=" << int(animation.trackTargetName(i))
                      << " type=" << int(animation.trackType(i))
                      << std::endl;
            continue;
        }

        clip.tracks.push_back(std::move(track));
    }

    return clip;
}

}

std::vector<AnimationClip3D> GltfAnimationLoader3D::loadAnimations(
    Magnum::Trade::AbstractImporter &importer
)
{
    std::vector<AnimationClip3D> animations;

    const std::optional<Magnum::UnsignedInt> idleAnimation =
        findAnimationByName(importer, PlayerIdleAnimationName);

    if (!idleAnimation.has_value()) {
        std::cerr << "[GLTF] cannot load idle animation, clip not found: "
                  << PlayerIdleAnimationName
                  << std::endl;
        return animations;
    }

    const Corrade::Containers::Optional<Magnum::Trade::AnimationData>
        animation = importer.animation(idleAnimation.value());

    if (!animation) {
        std::cerr << "[GLTF] failed to load idle animation "
                  << idleAnimation.value()
                  << std::endl;
        return animations;
    }

    AnimationClip3D clip = convertAnimationClip(
        importer,
        idleAnimation.value(),
        *animation
    );

    std::cout << "[GLTF] loaded animation "
              << idleAnimation.value()
              << " name=\"" << clip.name << "\""
              << " tracks=" << clip.tracks.size()
              << " duration=" << clip.duration
              << std::endl;

    if (!clip.isValid()) {
        std::cerr << "[GLTF] idle animation converted but is invalid"
                  << std::endl;
        return animations;
    }

    animations.push_back(std::move(clip));
    return animations;
}

}