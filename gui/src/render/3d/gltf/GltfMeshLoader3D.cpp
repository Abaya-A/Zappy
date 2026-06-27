#include "render/3d/gltf/GltfMeshLoader3D.hpp"

#include "render/3d/gltf/GltfMaterialLoader3D.hpp"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>

#include <Magnum/Trade/MeshData.h>

#include <iostream>
#include <utility>

namespace zappy::render3d {
namespace {

using JointIdsView =
    Corrade::Containers::StridedArrayView2D<const Magnum::UnsignedShort>;

using WeightsView =
    Corrade::Containers::StridedArrayView2D<const float>;

}

std::vector<Vertex3D> GltfMeshLoader3D::meshVertices(
    const Magnum::Trade::MeshData &mesh
)
{
    std::vector<Vertex3D> vertices;

    if (!mesh.hasAttribute(Magnum::Trade::MeshAttribute::Position))
        return vertices;

    const auto positions =
        mesh.attribute<Magnum::Vector3>(
            Magnum::Trade::MeshAttribute::Position
        );

    if (mesh.isIndexed()) {
        const Corrade::Containers::Array<Magnum::UnsignedInt> indices =
            mesh.indicesAsArray();

        vertices.reserve(indices.size());

        for (Magnum::UnsignedInt index : indices) {
            if (index >= positions.size())
                continue;

            vertices.push_back(Vertex3D{
                .position = positions[index]
            });
        }

        return vertices;
    }

    vertices.reserve(positions.size());

    for (const Magnum::Vector3 &position : positions) {
        vertices.push_back(Vertex3D{
            .position = position
        });
    }

    return vertices;
}

bool GltfMeshLoader3D::hasSkinningAttributes(
    const Magnum::Trade::MeshData &mesh
)
{
    return mesh.hasAttribute(Magnum::Trade::MeshAttribute::Position)
        && mesh.hasAttribute(Magnum::Trade::MeshAttribute::JointIds)
        && mesh.hasAttribute(Magnum::Trade::MeshAttribute::Weights);
}

void GltfMeshLoader3D::logMeshAttributes(
    const Magnum::Trade::MeshData &mesh,
    Magnum::UnsignedInt meshIndex
)
{
    std::cout << "[GLTF] mesh " << meshIndex
              << " attributeCount=" << mesh.attributeCount()
              << std::endl;

    for (Magnum::UnsignedInt i = 0; i < mesh.attributeCount(); ++i) {
        std::cout << "[GLTF] mesh " << meshIndex
                  << " attr " << i
                  << " name=" << static_cast<int>(mesh.attributeName(i))
                  << " id=" << mesh.attributeId(i)
                  << " format=" << static_cast<int>(mesh.attributeFormat(i))
                  << " arraySize=" << mesh.attributeArraySize(i)
                  << std::endl;
    }
}

std::vector<SkinnedVertex3D> GltfMeshLoader3D::skinnedMeshVertices(
    const Magnum::Trade::MeshData &mesh
)
{
    std::vector<SkinnedVertex3D> vertices;

    if (!hasSkinningAttributes(mesh))
        return vertices;

    const auto positions =
        mesh.attribute<Magnum::Vector3>(
            Magnum::Trade::MeshAttribute::Position
        );

    const JointIdsView jointIds =
        mesh.attribute<Magnum::UnsignedShort[]>(
            Magnum::Trade::MeshAttribute::JointIds
        );

    const WeightsView weights =
        mesh.attribute<float[]>(
            Magnum::Trade::MeshAttribute::Weights
        );

    std::cout << "[GLTF] skinned positions=" << positions.size()
              << " jointRows=" << jointIds.size()[0]
              << " jointCols=" << jointIds.size()[1]
              << " weightRows=" << weights.size()[0]
              << " weightCols=" << weights.size()[1]
              << std::endl;

    const auto appendVertex = [&](Magnum::UnsignedInt index) {
        if (index >= positions.size())
            return;

        if (index >= jointIds.size()[0] || index >= weights.size()[0])
            return;

        vertices.push_back(SkinnedVertex3D{
            .position = positions[index],
            .jointIds = Magnum::Vector4{
                static_cast<float>(jointIds[index][0]),
                static_cast<float>(jointIds[index][1]),
                static_cast<float>(jointIds[index][2]),
                static_cast<float>(jointIds[index][3])
            },
            .weights = Magnum::Vector4{
                weights[index][0],
                weights[index][1],
                weights[index][2],
                weights[index][3]
            }
        });
    };

    if (mesh.isIndexed()) {
        const Corrade::Containers::Array<Magnum::UnsignedInt> indices =
            mesh.indicesAsArray();

        vertices.reserve(indices.size());

        for (Magnum::UnsignedInt index : indices)
            appendVertex(index);

        return vertices;
    }

    vertices.reserve(positions.size());

    for (Magnum::UnsignedInt i = 0; i < positions.size(); ++i)
        appendVertex(i);

    return vertices;
}

std::vector<ModelSubMesh3D> GltfMeshLoader3D::loadSubMeshes(
    Magnum::Trade::AbstractImporter &importer,
    const std::vector<Magnum::Int> &meshMaterials
)
{
    std::vector<ModelSubMesh3D> subMeshes;

    for (Magnum::UnsignedInt i = 0; i < importer.meshCount(); ++i) {
        std::cout << "[GLTF] reading mesh " << i << std::endl;

        const Corrade::Containers::Optional<Magnum::Trade::MeshData> mesh =
            importer.mesh(i);

        if (!mesh) {
            std::cerr << "[GLTF] mesh " << i << " failed to load" << std::endl;
            continue;
        }

        std::vector<Vertex3D> vertices = meshVertices(*mesh);

        std::cout << "[GLTF] mesh=" << i
                  << " vertices=" << vertices.size()
                  << " indexed=" << mesh->isIndexed()
                  << std::endl;

        if (vertices.empty())
            continue;

        const Magnum::Int materialId =
            i < meshMaterials.size() ? meshMaterials[i] : -1;

        subMeshes.push_back(ModelSubMesh3D{
            .vertices = std::move(vertices),
            .color = GltfMaterialLoader3D::materialColor(
                importer,
                materialId,
                i
            )
        });
    }

    return subMeshes;
}

std::vector<SkinnedSubMesh3D> GltfMeshLoader3D::loadSkinnedSubMeshes(
    Magnum::Trade::AbstractImporter &importer,
    const std::vector<Magnum::Int> &meshMaterials
)
{
    std::vector<SkinnedSubMesh3D> skinnedSubMeshes;

    for (Magnum::UnsignedInt i = 0; i < importer.meshCount(); ++i) {
        const Corrade::Containers::Optional<Magnum::Trade::MeshData> mesh =
            importer.mesh(i);

        if (!mesh) {
            std::cerr << "[GLTF] skinned mesh " << i
                      << " failed to load"
                      << std::endl;
            continue;
        }

        if (!hasSkinningAttributes(*mesh)) {
            std::cout << "[GLTF] mesh " << i
                      << " has no skinning attributes, skipping skinned path"
                      << std::endl;
            continue;
        }

        logMeshAttributes(*mesh, i);

        std::vector<SkinnedVertex3D> vertices = skinnedMeshVertices(*mesh);

        std::cout << "[GLTF] skinned mesh=" << i
                  << " vertices=" << vertices.size()
                  << " indexed=" << mesh->isIndexed()
                  << std::endl;

        if (vertices.empty())
            continue;

        const Magnum::Int materialId =
            i < meshMaterials.size() ? meshMaterials[i] : -1;

        skinnedSubMeshes.push_back(SkinnedSubMesh3D{
            .vertices = std::move(vertices),
            .color = GltfMaterialLoader3D::materialColor(
                importer,
                materialId,
                i
            ),
            .skinId = 0,
            .nodeTransform = Magnum::Matrix4{Magnum::Math::IdentityInit}
        });
    }

    return skinnedSubMeshes;
}

}