#include "render/3d/MtlMaterialLoader3D.hpp"

#include <fstream>
#include <sstream>
#include <string>

namespace zappy::render3d {
namespace {

bool startsWith(const std::string &line, const std::string &prefix)
{
    return line.rfind(prefix, 0) == 0;
}

}

MtlMaterialLoader3D::MaterialMap MtlMaterialLoader3D::load(
    const std::string &path
) const
{
    MaterialMap materials;
    std::ifstream file(path);

    if (!file.is_open())
        return materials;

    std::string currentMaterial;
    std::string line;

    while (std::getline(file, line)) {
        if (startsWith(line, "newmtl ")) {
            std::istringstream stream(line);
            std::string keyword;

            stream >> keyword >> currentMaterial;
        } else if (startsWith(line, "Kd ") && !currentMaterial.empty()) {
            std::istringstream stream(line);
            std::string keyword;
            float r = 1.0f;
            float g = 1.0f;
            float b = 1.0f;

            if (stream >> keyword >> r >> g >> b)
                materials[currentMaterial] = Magnum::Color4{r, g, b, 1.0f};
        }
    }

    return materials;
}

}