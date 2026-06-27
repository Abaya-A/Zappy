#include "render/3d/SkinnedAnimation3D.hpp"

#include <Magnum/Math/Functions.h>
#include <Magnum/Math/Matrix4.h>

#include <cmath>
#include <cstddef>
#include <vector>

namespace zappy::render3d {
namespace {

float wrappedTime(float timeSeconds, float duration)
{
    if (duration <= 0.0f)
        return 0.0f;

    const float wrapped = std::fmod(timeSeconds, duration);

    if (wrapped < 0.0f)
        return wrapped + duration;

    return wrapped;
}

Magnum::Quaternion identityRotation()
{
    return Magnum::Quaternion{
        Magnum::Vector3{0.0f, 0.0f, 0.0f},
        1.0f
    };
}

Magnum::Matrix4 transformMatrix(
    const Magnum::Vector3 &translation,
    const Magnum::Quaternion &rotation,
    const Magnum::Vector3 &scaling
)
{
    return Magnum::Matrix4::translation(translation)
        * Magnum::Matrix4::from(rotation.normalized().toMatrix(), {})
        * Magnum::Matrix4::scaling(scaling);
}

template<typename Key>
std::size_t previousKeyIndex(const std::vector<Key> &keys, float timeSeconds)
{
    if (keys.empty())
        return 0;

    for (std::size_t i = 1; i < keys.size(); ++i) {
        if (timeSeconds < keys[i].time)
            return i - 1;
    }

    return keys.size() - 1;
}

template<typename Key>
std::size_t nextKeyIndex(const std::vector<Key> &keys, std::size_t previous)
{
    if (keys.empty())
        return 0;

    if (previous + 1 >= keys.size())
        return previous;

    return previous + 1;
}

float interpolationFactor(float previousTime, float nextTime, float timeSeconds)
{
    const float span = nextTime - previousTime;

    if (span <= 0.0f)
        return 0.0f;

    return (timeSeconds - previousTime) / span;
}

Magnum::Matrix4 computeGlobalTransform(
    std::size_t nodeId,
    const std::vector<SceneNode3D> &nodes,
    const std::vector<Magnum::Matrix4> &locals,
    std::vector<Magnum::Matrix4> &globals,
    std::vector<bool> &computed
)
{
    if (computed[nodeId])
        return globals[nodeId];

    const int parent = nodes[nodeId].parent;

    if (parent < 0 || static_cast<std::size_t>(parent) >= nodes.size()) {
        globals[nodeId] = locals[nodeId];
    } else {
        const std::size_t parentId = static_cast<std::size_t>(parent);

        globals[nodeId] =
            computeGlobalTransform(
                parentId,
                nodes,
                locals,
                globals,
                computed
            ) * locals[nodeId];
    }

    computed[nodeId] = true;
    return globals[nodeId];
}

}

Magnum::Vector3 SkinnedAnimation3D::sampleTranslation(
    const AnimationTrack3D &track,
    float timeSeconds
)
{
    if (track.translations.empty())
        return Magnum::Vector3{0.0f};

    const std::size_t previous =
        previousKeyIndex(track.translations, timeSeconds);
    const std::size_t next =
        nextKeyIndex(track.translations, previous);

    const AnimationKeyTranslation3D &a = track.translations[previous];
    const AnimationKeyTranslation3D &b = track.translations[next];

    const float t = interpolationFactor(a.time, b.time, timeSeconds);

    return Magnum::Math::lerp(a.value, b.value, t);
}

Magnum::Quaternion SkinnedAnimation3D::sampleRotation(
    const AnimationTrack3D &track,
    float timeSeconds
)
{
    if (track.rotations.empty())
        return identityRotation();

    const std::size_t previous =
        previousKeyIndex(track.rotations, timeSeconds);
    const std::size_t next =
        nextKeyIndex(track.rotations, previous);

    const AnimationKeyRotation3D &a = track.rotations[previous];
    const AnimationKeyRotation3D &b = track.rotations[next];

    const float t = interpolationFactor(a.time, b.time, timeSeconds);

    return Magnum::Math::slerp(a.value, b.value, t).normalized();
}

Magnum::Vector3 SkinnedAnimation3D::sampleScaling(
    const AnimationTrack3D &track,
    float timeSeconds
)
{
    if (track.scalings.empty())
        return Magnum::Vector3{1.0f};

    const std::size_t previous =
        previousKeyIndex(track.scalings, timeSeconds);
    const std::size_t next =
        nextKeyIndex(track.scalings, previous);

    const AnimationKeyScaling3D &a = track.scalings[previous];
    const AnimationKeyScaling3D &b = track.scalings[next];

    const float t = interpolationFactor(a.time, b.time, timeSeconds);

    return Magnum::Math::lerp(a.value, b.value, t);
}

std::vector<Magnum::Matrix4> SkinnedAnimation3D::animatedLocalTransforms(
    const Model3D &model,
    const AnimationClip3D &clip,
    float timeSeconds
)
{
    std::vector<Magnum::Matrix4> locals;

    locals.reserve(model.nodes().size());

    for (const SceneNode3D &node : model.nodes())
        locals.push_back(node.localTransform);

    const float time = wrappedTime(timeSeconds, clip.duration);

    std::vector<Magnum::Vector3> translations;
    std::vector<Magnum::Quaternion> rotations;
    std::vector<Magnum::Vector3> scalings;

    std::vector<bool> hasTranslation(model.nodes().size(), false);
    std::vector<bool> hasRotation(model.nodes().size(), false);
    std::vector<bool> hasScaling(model.nodes().size(), false);

    translations.reserve(model.nodes().size());
    rotations.reserve(model.nodes().size());
    scalings.reserve(model.nodes().size());

    for (const SceneNode3D &node : model.nodes()) {
        /*
         * Critical fix:
         * Keep the bind-pose local translation by default.
         * Most skeleton animation clips animate rotation only.
         * If we default translation to {0,0,0}, bones collapse to their parent.
         */
        translations.push_back(node.localTransform.translation());
        rotations.push_back(identityRotation());
        scalings.push_back(Magnum::Vector3{1.0f});
    }

    for (const AnimationTrack3D &track : clip.tracks) {
        if (track.objectId >= locals.size())
            continue;

        const std::size_t objectId =
            static_cast<std::size_t>(track.objectId);

        if (track.target == AnimationTrackTarget3D::Translation) {
            translations[objectId] = sampleTranslation(track, time);
            hasTranslation[objectId] = true;
        }

        if (track.target == AnimationTrackTarget3D::Rotation) {
            rotations[objectId] = sampleRotation(track, time);
            hasRotation[objectId] = true;
        }

        if (track.target == AnimationTrackTarget3D::Scaling) {
            scalings[objectId] = sampleScaling(track, time);
            hasScaling[objectId] = true;
        }
    }

    for (std::size_t i = 0; i < locals.size(); ++i) {
        if (!hasTranslation[i] && !hasRotation[i] && !hasScaling[i])
            continue;

        locals[i] = transformMatrix(
            translations[i],
            rotations[i],
            scalings[i]
        );
    }

    return locals;
}

std::vector<Magnum::Matrix4> SkinnedAnimation3D::globalTransforms(
    const std::vector<SceneNode3D> &nodes,
    const std::vector<Magnum::Matrix4> &locals
)
{
    std::vector<Magnum::Matrix4> globals(
        nodes.size(),
        Magnum::Matrix4{Magnum::Math::IdentityInit}
    );

    std::vector<bool> computed(nodes.size(), false);

    for (std::size_t i = 0; i < nodes.size(); ++i) {
        computeGlobalTransform(
            i,
            nodes,
            locals,
            globals,
            computed
        );
    }

    return globals;
}

std::vector<Magnum::Matrix4> SkinnedAnimation3D::jointMatrices(
    const Model3D &model,
    float timeSeconds
)
{
    std::vector<Magnum::Matrix4> matrices;

    if (model.animations().empty()
        || model.nodes().empty()
        || model.skins().empty())
        return matrices;

    const AnimationClip3D &clip = model.animations().front();
    const Skin3D &skin = model.skins().front();

    const std::vector<Magnum::Matrix4> locals =
        animatedLocalTransforms(model, clip, timeSeconds);

    const std::vector<Magnum::Matrix4> globals =
        globalTransforms(model.nodes(), locals);

    matrices.reserve(skin.joints.size());

    for (std::size_t i = 0; i < skin.joints.size(); ++i) {
        const unsigned int jointObjectId = skin.joints[i];

        if (jointObjectId >= globals.size()
            || i >= skin.inverseBindMatrices.size()) {
            matrices.push_back(Magnum::Matrix4{Magnum::Math::IdentityInit});
            continue;
        }

        matrices.push_back(
            globals[jointObjectId] * skin.inverseBindMatrices[i]
        );
    }

    return matrices;
}

}