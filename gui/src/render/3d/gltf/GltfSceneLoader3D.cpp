#include "render/3d/gltf/GltfSceneLoader3D.hpp"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>

#include <Magnum/Trade/SceneData.h>

#include <iostream>

namespace zappy::render3d {

std::vector<SceneNode3D> GltfSceneLoader3D::loadSceneNodes(
    Magnum::Trade::AbstractImporter &importer
)
{
    std::vector<SceneNode3D> nodes;

    if (importer.defaultScene() == -1) {
        std::cerr << "[GLTF] no default scene, cannot preserve scene nodes"
                  << std::endl;
        return nodes;
    }

    const Corrade::Containers::Optional<Magnum::Trade::SceneData> scene =
        importer.scene(importer.defaultScene());

    if (!scene) {
        std::cerr << "[GLTF] failed to load scene "
                  << importer.defaultScene()
                  << std::endl;
        return nodes;
    }

    if (!scene->is3D()) {
        std::cerr << "[GLTF] scene is not 3D" << std::endl;
        return nodes;
    }

    if (!scene->hasField(Magnum::Trade::SceneField::Parent)) {
        std::cerr << "[GLTF] scene has no parent hierarchy" << std::endl;
        return nodes;
    }

    nodes.resize(static_cast<std::size_t>(scene->mappingBound()));

    for (SceneNode3D &node : nodes) {
        node.parent = -1;
        node.localTransform = Magnum::Matrix4{Magnum::Math::IdentityInit};
        node.meshIds.clear();
    }

    const Corrade::Containers::Array<
        Corrade::Containers::Pair<Magnum::UnsignedInt, Magnum::Int>
    > parents = scene->parentsAsArray();

    for (const Corrade::Containers::Pair<Magnum::UnsignedInt, Magnum::Int> &parent
        : parents) {
        const Magnum::UnsignedInt objectId = parent.first();

        if (objectId >= nodes.size()) {
            std::cerr << "[GLTF] invalid parent object id "
                      << objectId
                      << std::endl;
            continue;
        }

        nodes[objectId].parent = parent.second();
    }

    const Corrade::Containers::Array<
        Corrade::Containers::Pair<Magnum::UnsignedInt, Magnum::Matrix4>
    > transformations = scene->transformations3DAsArray();

    for (const Corrade::Containers::Pair<Magnum::UnsignedInt, Magnum::Matrix4>
        &transform : transformations) {
        const Magnum::UnsignedInt objectId = transform.first();

        if (objectId >= nodes.size()) {
            std::cerr << "[GLTF] invalid transform object id "
                      << objectId
                      << std::endl;
            continue;
        }

        nodes[objectId].localTransform = transform.second();
    }

    if (scene->hasField(Magnum::Trade::SceneField::Mesh)) {
        const Corrade::Containers::Array<
            Corrade::Containers::Pair<
                Magnum::UnsignedInt,
                Corrade::Containers::Pair<Magnum::UnsignedInt, Magnum::Int>
            >
        > meshes = scene->meshesMaterialsAsArray();

        for (const Corrade::Containers::Pair<
            Magnum::UnsignedInt,
            Corrade::Containers::Pair<Magnum::UnsignedInt, Magnum::Int>
        > &mesh : meshes) {
            const Magnum::UnsignedInt objectId = mesh.first();
            const Magnum::UnsignedInt meshId = mesh.second().first();

            if (objectId >= nodes.size()) {
                std::cerr << "[GLTF] invalid mesh object id "
                          << objectId
                          << std::endl;
                continue;
            }

            nodes[objectId].meshIds.push_back(meshId);
        }
    }

    std::cout << "[GLTF] loaded scene nodes=" << nodes.size()
              << " hierarchyEntries=" << parents.size()
              << std::endl;

    return nodes;
}

}