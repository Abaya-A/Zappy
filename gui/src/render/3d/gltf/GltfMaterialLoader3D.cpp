#include "render/3d/gltf/GltfMaterialLoader3D.hpp"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>

#include <Magnum/Trade/MaterialData.h>
#include <Magnum/Trade/SceneData.h>

#include <iostream>

namespace zappy::render3d {

Magnum::Color4 GltfMaterialLoader3D::debugMeshColor(
    Magnum::UnsignedInt meshIndex
)
{
    switch (meshIndex % 6) {
        case 0:
            return Magnum::Color4{0.75f, 0.85f, 1.0f, 1.0f};
        case 1:
            return Magnum::Color4{1.0f, 0.75f, 0.85f, 1.0f};
        case 2:
            return Magnum::Color4{0.85f, 1.0f, 0.75f, 1.0f};
        case 3:
            return Magnum::Color4{1.0f, 0.9f, 0.55f, 1.0f};
        case 4:
            return Magnum::Color4{0.75f, 1.0f, 0.95f, 1.0f};
        default:
            return Magnum::Color4{0.9f, 0.75f, 1.0f, 1.0f};
    }
}

Magnum::Color4 GltfMaterialLoader3D::materialColor(
    Magnum::Trade::AbstractImporter &importer,
    Magnum::Int materialId,
    Magnum::UnsignedInt fallbackMeshIndex
)
{
    if (materialId < 0)
        return debugMeshColor(fallbackMeshIndex);

    const Corrade::Containers::Optional<Magnum::Trade::MaterialData> material =
        importer.material(static_cast<Magnum::UnsignedInt>(materialId));

    if (!material) {
        std::cerr << "[GLTF] failed to load material "
                  << materialId
                  << std::endl;
        return debugMeshColor(fallbackMeshIndex);
    }

    if (!material->hasAttribute(Magnum::Trade::MaterialAttribute::BaseColor)) {
        std::cout << "[GLTF] material " << materialId
                  << " has no BaseColor, using fallback"
                  << std::endl;
        return debugMeshColor(fallbackMeshIndex);
    }

    const Magnum::Color4 color =
        material->attribute<Magnum::Color4>(
            Magnum::Trade::MaterialAttribute::BaseColor
        );

    std::cout << "[GLTF] material " << materialId
              << " baseColor=("
              << color.r() << ", "
              << color.g() << ", "
              << color.b() << ", "
              << color.a() << ")"
              << std::endl;

    return color;
}

std::vector<Magnum::Int> GltfMaterialLoader3D::loadMeshMaterials(
    Magnum::Trade::AbstractImporter &importer
)
{
    std::vector<Magnum::Int> meshMaterials(
        importer.meshCount(),
        -1
    );

    if (importer.defaultScene() == -1)
        return meshMaterials;

    const Corrade::Containers::Optional<Magnum::Trade::SceneData> scene =
        importer.scene(importer.defaultScene());

    if (!scene)
        return meshMaterials;

    if (!scene->hasField(Magnum::Trade::SceneField::Mesh))
        return meshMaterials;

    const Corrade::Containers::Array<
        Corrade::Containers::Pair<
            Magnum::UnsignedInt,
            Corrade::Containers::Pair<Magnum::UnsignedInt, Magnum::Int>
        >
    > meshes = scene->meshesMaterialsAsArray();

    for (const Corrade::Containers::Pair<
        Magnum::UnsignedInt,
        Corrade::Containers::Pair<Magnum::UnsignedInt, Magnum::Int>
    > &entry : meshes) {
        const Magnum::UnsignedInt meshId = entry.second().first();
        const Magnum::Int materialId = entry.second().second();

        if (meshId >= meshMaterials.size())
            continue;

        meshMaterials[meshId] = materialId;

        std::cout << "[GLTF] meshMaterial mesh="
                  << meshId
                  << " material="
                  << materialId
                  << std::endl;
    }

    return meshMaterials;
}

}