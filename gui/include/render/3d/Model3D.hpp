#pragma once

#include "render/3d/RenderGeometry3D.hpp"

#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Quaternion.h>
#include <Magnum/Math/Vector3.h>

#include <string>
#include <vector>

namespace zappy::render3d {

struct ModelSubMesh3D {
    std::vector<Vertex3D> vertices;
    Magnum::Color4 color;
};

struct SkinnedSubMesh3D {
    std::vector<SkinnedVertex3D> vertices;
    Magnum::Color4 color;
    int skinId;
    Magnum::Matrix4 nodeTransform;
};

struct Skin3D {
    std::vector<unsigned int> joints;
    std::vector<Magnum::Matrix4> inverseBindMatrices;
};

enum class AnimationTrackTarget3D {
    Translation,
    Rotation,
    Scaling
};

struct AnimationKeyTranslation3D {
    float time;
    Magnum::Vector3 value;
};

struct AnimationKeyRotation3D {
    float time;
    Magnum::Quaternion value;
};

struct AnimationKeyScaling3D {
    float time;
    Magnum::Vector3 value;
};

struct AnimationTrack3D {
    unsigned int objectId;
    AnimationTrackTarget3D target;

    std::vector<AnimationKeyTranslation3D> translations;
    std::vector<AnimationKeyRotation3D> rotations;
    std::vector<AnimationKeyScaling3D> scalings;
};

struct AnimationClip3D {
    std::string name;
    float duration;
    std::vector<AnimationTrack3D> tracks;

    bool isValid() const;
};

struct SceneNode3D {
    int parent;
    Magnum::Matrix4 localTransform;
    std::vector<unsigned int> meshIds;
};

class Model3D {
public:
    Model3D();
    explicit Model3D(std::vector<Vertex3D> vertices);
    explicit Model3D(std::vector<ModelSubMesh3D> subMeshes);

    Model3D(
        std::vector<ModelSubMesh3D> subMeshes,
        std::vector<SceneNode3D> nodes,
        std::vector<AnimationClip3D> animations
    );

    Model3D(
        std::vector<ModelSubMesh3D> subMeshes,
        std::vector<SkinnedSubMesh3D> skinnedSubMeshes,
        std::vector<Skin3D> skins,
        std::vector<SceneNode3D> nodes,
        std::vector<AnimationClip3D> animations
    );

    bool isValid() const;
    bool hasAnimations() const;
    bool isSkinned() const;

    const std::vector<Vertex3D> &vertices() const;
    const std::vector<ModelSubMesh3D> &subMeshes() const;
    const std::vector<SkinnedSubMesh3D> &skinnedSubMeshes() const;
    const std::vector<Skin3D> &skins() const;
    const std::vector<SceneNode3D> &nodes() const;
    const std::vector<AnimationClip3D> &animations() const;

private:
    std::vector<Vertex3D> _vertices;
    std::vector<ModelSubMesh3D> _subMeshes;
    std::vector<SkinnedSubMesh3D> _skinnedSubMeshes;
    std::vector<Skin3D> _skins;
    std::vector<SceneNode3D> _nodes;
    std::vector<AnimationClip3D> _animations;
};

}