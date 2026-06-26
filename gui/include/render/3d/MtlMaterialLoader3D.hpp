#pragma once

#include <Magnum/Math/Color.h>

#include <string>
#include <unordered_map>

namespace zappy::render3d {

class MtlMaterialLoader3D {
public:
    using MaterialMap = std::unordered_map<std::string, Magnum::Color4>;

    MaterialMap load(const std::string &path) const;
};

}