#pragma once

#include "render/3d/Model3D.hpp"

#include <optional>
#include <string>

namespace zappy::render3d {

class GltfModelLoader3D {
public:
    std::optional<Model3D> load(const std::string &path) const;
};

}