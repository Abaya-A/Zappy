#include "render/3d/GltfModelLoader3D.hpp"

#include "render/3d/gltf/GltfAnimationLoader3D.hpp"
#include "render/3d/gltf/GltfMaterialLoader3D.hpp"
#include "render/3d/gltf/GltfMeshLoader3D.hpp"
#include "render/3d/gltf/GltfSceneLoader3D.hpp"
#include "render/3d/gltf/GltfSkinLoader3D.hpp"

#include <Corrade/Containers/Pointer.h>
#include <Corrade/PluginManager/Manager.h>

#include <Magnum/Trade/AbstractImporter.h>

#include <iostream>
#include <optional>
#include <utility>
#include <vector>

namespace zappy::render3d {

std::optional<Model3D> GltfModelLoader3D::load(
    const std::string &path
) const
{
    Corrade::PluginManager::Manager<Magnum::Trade::AbstractImporter> manager;

    Corrade::Containers::Pointer<Magnum::Trade::AbstractImporter> importer =
        manager.loadAndInstantiate("GltfImporter");

    if (!importer) {
        std::cerr << "[GLTF] failed to load GltfImporter" << std::endl;
        return std::nullopt;
    }

    std::cout << "[GLTF] loading " << path << std::endl;

    if (!importer->openFile(path)) {
        std::cerr << "[GLTF] failed to open " << path << std::endl;
        return std::nullopt;
    }

    std::cout << "[GLTF] opened file" << std::endl;
    std::cout << "[GLTF] sceneCount=" << importer->sceneCount() << std::endl;
    std::cout << "[GLTF] objectCount=" << importer->objectCount() << std::endl;
    std::cout << "[GLTF] animationCount=" << importer->animationCount() << std::endl;
    std::cout << "[GLTF] skin3DCount=" << importer->skin3DCount() << std::endl;
    std::cout << "[GLTF] materialCount=" << importer->materialCount() << std::endl;
    std::cout << "[GLTF] meshCount=" << importer->meshCount() << std::endl;

    std::vector<Magnum::Int> meshMaterials =
        GltfMaterialLoader3D::loadMeshMaterials(*importer);

    std::vector<ModelSubMesh3D> subMeshes =
        GltfMeshLoader3D::loadSubMeshes(*importer, meshMaterials);

    std::vector<SkinnedSubMesh3D> skinnedSubMeshes =
        GltfMeshLoader3D::loadSkinnedSubMeshes(*importer, meshMaterials);

    std::vector<Skin3D> skins =
        GltfSkinLoader3D::loadSkins(*importer);

    std::vector<SceneNode3D> nodes =
        GltfSceneLoader3D::loadSceneNodes(*importer);

    std::vector<AnimationClip3D> animations =
        GltfAnimationLoader3D::loadAnimations(*importer);

    std::cout << "[INFO]: loaded glTF model: "
              << path
              << " meshes=" << subMeshes.size()
              << " skinnedMeshes=" << skinnedSubMeshes.size()
              << " skins=" << skins.size()
              << " nodes=" << nodes.size()
              << " animations=" << animations.size()
              << std::endl;

    return Model3D{
        std::move(subMeshes),
        std::move(skinnedSubMeshes),
        std::move(skins),
        std::move(nodes),
        std::move(animations)
    };
}

}