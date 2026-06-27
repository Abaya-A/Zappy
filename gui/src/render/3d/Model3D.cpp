#include "render/3d/Model3D.hpp"

#include <utility>

namespace zappy::render3d {

Model3D::Model3D()
    : _vertices(),
      _subMeshes(),
      _skinnedSubMeshes(),
      _skins(),
      _nodes(),
      _animations()
{
}

Model3D::Model3D(std::vector<Vertex3D> vertices)
    : _vertices(std::move(vertices)),
      _subMeshes(),
      _skinnedSubMeshes(),
      _skins(),
      _nodes(),
      _animations()
{
}

Model3D::Model3D(std::vector<ModelSubMesh3D> subMeshes)
    : _vertices(),
      _subMeshes(std::move(subMeshes)),
      _skinnedSubMeshes(),
      _skins(),
      _nodes(),
      _animations()
{
}

Model3D::Model3D(
    std::vector<ModelSubMesh3D> subMeshes,
    std::vector<SceneNode3D> nodes,
    std::vector<AnimationClip3D> animations
)
    : _vertices(),
      _subMeshes(std::move(subMeshes)),
      _skinnedSubMeshes(),
      _skins(),
      _nodes(std::move(nodes)),
      _animations(std::move(animations))
{
}

Model3D::Model3D(
    std::vector<ModelSubMesh3D> subMeshes,
    std::vector<SkinnedSubMesh3D> skinnedSubMeshes,
    std::vector<Skin3D> skins,
    std::vector<SceneNode3D> nodes,
    std::vector<AnimationClip3D> animations
)
    : _vertices(),
      _subMeshes(std::move(subMeshes)),
      _skinnedSubMeshes(std::move(skinnedSubMeshes)),
      _skins(std::move(skins)),
      _nodes(std::move(nodes)),
      _animations(std::move(animations))
{
}

bool AnimationClip3D::isValid() const
{
    return duration > 0.0f && !tracks.empty();
}

bool Model3D::isValid() const
{
    if (!_vertices.empty())
        return true;

    for (const ModelSubMesh3D &subMesh : _subMeshes) {
        if (!subMesh.vertices.empty())
            return true;
    }

    for (const SkinnedSubMesh3D &subMesh : _skinnedSubMeshes) {
        if (!subMesh.vertices.empty())
            return true;
    }

    return false;
}

bool Model3D::hasAnimations() const
{
    for (const AnimationClip3D &animation : _animations) {
        if (animation.isValid())
            return true;
    }

    return false;
}

bool Model3D::isSkinned() const
{
    return !_skinnedSubMeshes.empty() && !_skins.empty();
}

const std::vector<Vertex3D> &Model3D::vertices() const
{
    return _vertices;
}

const std::vector<ModelSubMesh3D> &Model3D::subMeshes() const
{
    return _subMeshes;
}

const std::vector<SkinnedSubMesh3D> &Model3D::skinnedSubMeshes() const
{
    return _skinnedSubMeshes;
}

const std::vector<Skin3D> &Model3D::skins() const
{
    return _skins;
}

const std::vector<SceneNode3D> &Model3D::nodes() const
{
    return _nodes;
}

const std::vector<AnimationClip3D> &Model3D::animations() const
{
    return _animations;
}

}