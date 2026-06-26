#pragma once

#include "render/3d/Model3D.hpp"
#include "render/3d/MtlMaterialLoader3D.hpp"

#include <Magnum/Math/Color.h>
#include <Magnum/Math/Vector3.h>

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace zappy::render3d {

class ObjModelLoader3D {
public:
    std::optional<Model3D> load(const std::string &path) const;

private:
    struct ObjParseContext {
        std::vector<Magnum::Vector3> positions;
        std::unordered_map<std::string, std::vector<Vertex3D>> materialVertices;
        MtlMaterialLoader3D::MaterialMap materials;
        std::string currentMaterial = "default";
    };

    static bool parseVertexLine(const std::string &line, Magnum::Vector3 &vertex);

    static bool parseFaceLine(
        const std::string &line,
        const std::vector<Magnum::Vector3> &positions,
        std::vector<Vertex3D> &vertices
    );

    static std::optional<int> parseFaceIndex(
        const std::string &token,
        int vertexCount
    );

    static std::string parentDirectory(const std::string &path);
    static std::string materialPath(const std::string &objPath, const std::string &mtlFile);
    static std::optional<Model3D> buildModel(const ObjParseContext &context);
};

}