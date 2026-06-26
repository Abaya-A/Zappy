#include "render/3d/GltfModelLoader3D.hpp"

#include <Corrade/Containers/Array.h>
#include <Magnum/Magnum.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pointer.h>
#include <Corrade/PluginManager/Manager.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/MaterialData.h>

#include <iostream>
#include <optional>
#include <vector>

namespace zappy::render3d {
namespace {

constexpr Magnum::Color4 DefaultColor{0.75f, 0.85f, 1.0f, 1.0f};

Magnum::Color4 debugMeshColor(Magnum::UnsignedInt meshIndex)
{
    static const Magnum::Color4 colors[] = {
        Magnum::Color4{0.45f, 0.85f, 1.00f, 1.0f},
        Magnum::Color4{0.95f, 0.35f, 0.65f, 1.0f},
        Magnum::Color4{0.85f, 0.95f, 0.35f, 1.0f},
        Magnum::Color4{0.80f, 0.65f, 1.00f, 1.0f}
    };

    return colors[meshIndex % 4];
}

std::vector<Vertex3D> meshVertices(const Magnum::Trade::MeshData &mesh)
{
    std::vector<Vertex3D> vertices;

    if (mesh.primitive() != Magnum::MeshPrimitive::Triangles) {
        std::cerr << "[GLTF] unsupported primitive, only triangles supported" << std::endl;
        return vertices;
    }

    if (!mesh.hasAttribute(Magnum::Trade::MeshAttribute::Position)) {
        std::cerr << "[GLTF] mesh has no position attribute" << std::endl;
        return vertices;
    }

    const auto positions = mesh.attribute<Magnum::Vector3>(
        Magnum::Trade::MeshAttribute::Position
    );

    if (!mesh.isIndexed()) {
        vertices.reserve(positions.size());

        for (const Magnum::Vector3 &position : positions)
            vertices.push_back({position});

        return vertices;
    }

    const Corrade::Containers::Array<Magnum::UnsignedInt> indices =
        mesh.indicesAsArray();

    vertices.reserve(indices.size());

    for (const Magnum::UnsignedInt index : indices) {
        if (index >= positions.size()) {
            std::cerr << "[GLTF] invalid index " << index << std::endl;
            continue;
        }

        vertices.push_back({positions[index]});
    }

    return vertices;
}

}

std::optional<Model3D> GltfModelLoader3D::load(const std::string &path) const
{
    std::cout << "[GLTF] loading " << path << std::endl;

    Corrade::PluginManager::Manager<Magnum::Trade::AbstractImporter> manager;
    Corrade::Containers::Pointer<Magnum::Trade::AbstractImporter> importer =
        manager.loadAndInstantiate("GltfImporter");

    if (!importer) {
        std::cerr << "[WARN]: failed to load GltfImporter plugin" << std::endl;
        return std::nullopt;
    }

    if (!importer->openFile(path)) {
        std::cerr << "[WARN]: failed to open glTF model: " << path << std::endl;
        return std::nullopt;
    }

    std::cout << "[GLTF] opened file" << std::endl;
    std::cout << "[GLTF] meshCount=" << importer->meshCount() << std::endl;

    std::vector<ModelSubMesh3D> subMeshes;

    for (Magnum::UnsignedInt i = 0; i < importer->meshCount(); ++i) {
        std::cout << "[GLTF] reading mesh " << i << std::endl;

        const auto mesh = importer->mesh(i);

        if (!mesh) {
            std::cout << "[GLTF] mesh " << i << " failed to load" << std::endl;
            continue;
        }

        std::vector<Vertex3D> vertices = meshVertices(*mesh);

        std::cout << "[GLTF] mesh=" << i
                  << " vertices=" << vertices.size()
                  << " indexed=" << mesh->isIndexed()
                  << std::endl;

        if (vertices.empty())
            continue;

        subMeshes.push_back(ModelSubMesh3D{
            .vertices = std::move(vertices),
            .color = debugMeshColor(i)
        });
    }

    if (subMeshes.empty()) {
        std::cerr << "[WARN]: glTF model has no supported meshes: " << path << std::endl;
        return std::nullopt;
    }

    std::cout << "[INFO]: loaded glTF model: " << path
              << " meshes=" << subMeshes.size() << std::endl;

    return Model3D(std::move(subMeshes));
}

}