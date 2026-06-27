#include "render/3d/gltf/GltfSkinLoader3D.hpp"

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>

#include <Magnum/Trade/SkinData.h>

#include <iostream>

namespace zappy::render3d {

std::vector<Skin3D> GltfSkinLoader3D::loadSkins(
    Magnum::Trade::AbstractImporter &importer
)
{
    std::vector<Skin3D> skins;

    for (Magnum::UnsignedInt i = 0; i < importer.skin3DCount(); ++i) {
        const Corrade::Containers::Optional<Magnum::Trade::SkinData3D> skin =
            importer.skin3D(i);

        if (!skin) {
            std::cerr << "[GLTF] skin " << i << " failed to load" << std::endl;
            continue;
        }

        Skin3D modelSkin;

        const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>
            joints = skin->joints();

        const Corrade::Containers::StridedArrayView1D<const Magnum::Matrix4>
            inverseBindMatrices = skin->inverseBindMatrices();

        modelSkin.joints.reserve(joints.size());
        for (const Magnum::UnsignedInt joint : joints)
            modelSkin.joints.push_back(joint);

        modelSkin.inverseBindMatrices.reserve(inverseBindMatrices.size());
        for (const Magnum::Matrix4 &matrix : inverseBindMatrices)
            modelSkin.inverseBindMatrices.push_back(matrix);

        std::cout << "[GLTF] skin=" << i
                  << " joints=" << modelSkin.joints.size()
                  << " inverseBindMatrices="
                  << modelSkin.inverseBindMatrices.size()
                  << std::endl;

        skins.push_back(std::move(modelSkin));
    }

    return skins;
}

}