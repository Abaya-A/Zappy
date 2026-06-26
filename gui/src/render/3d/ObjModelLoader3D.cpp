#include "render/3d/ObjModelLoader3D.hpp"

#include <charconv>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>

namespace zappy::render3d {
namespace {

constexpr Magnum::Color4 DefaultColor{1.0f, 1.0f, 1.0f, 1.0f};

bool startsWith(const std::string &line, const std::string &prefix)
{
    return line.rfind(prefix, 0) == 0;
}

std::string firstFaceTokenPart(const std::string &token)
{
    const std::size_t slash = token.find('/');

    if (slash == std::string::npos)
        return token;
    return token.substr(0, slash);
}

}

std::optional<Model3D> ObjModelLoader3D::load(const std::string &path) const
{
    std::ifstream file(path);

    if (!file.is_open())
        return std::nullopt;

    ObjParseContext context;
    std::string line;

    while (std::getline(file, line)) {
        if (startsWith(line, "mtllib ")) {
            std::istringstream stream(line);
            std::string keyword;
            std::string mtlFile;

            stream >> keyword >> mtlFile;
            context.materials = MtlMaterialLoader3D{}.load(
                materialPath(path, mtlFile)
            );
        } else if (startsWith(line, "usemtl ")) {
            std::istringstream stream(line);
            std::string keyword;

            stream >> keyword >> context.currentMaterial;
        } else if (startsWith(line, "v ")) {
            Magnum::Vector3 vertex;

            if (!parseVertexLine(line, vertex))
                return std::nullopt;
            context.positions.push_back(vertex);
        } else if (startsWith(line, "f ")) {
            if (!parseFaceLine(
                    line,
                    context.positions,
                    context.materialVertices[context.currentMaterial]
                ))
                return std::nullopt;
        }
    }

    return buildModel(context);
}

bool ObjModelLoader3D::parseVertexLine(
    const std::string &line,
    Magnum::Vector3 &vertex
)
{
    std::istringstream stream(line);
    std::string prefix;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    if (!(stream >> prefix >> x >> y >> z))
        return false;

    vertex = Magnum::Vector3{x, y, z};
    return true;
}

bool ObjModelLoader3D::parseFaceLine(
    const std::string &line,
    const std::vector<Magnum::Vector3> &positions,
    std::vector<Vertex3D> &vertices
)
{
    std::istringstream stream(line);
    std::string prefix;
    std::vector<int> indices;
    std::string token;

    stream >> prefix;

    while (stream >> token) {
        const std::optional<int> index = parseFaceIndex(
            token,
            static_cast<int>(positions.size())
        );

        if (!index.has_value())
            return false;
        indices.push_back(index.value());
    }

    if (indices.size() < 3)
        return false;

    for (std::size_t i = 1; i + 1 < indices.size(); ++i) {
        vertices.push_back({positions[static_cast<std::size_t>(indices[0])]});
        vertices.push_back({positions[static_cast<std::size_t>(indices[i])]});
        vertices.push_back({positions[static_cast<std::size_t>(indices[i + 1])]});
    }

    return true;
}

std::optional<int> ObjModelLoader3D::parseFaceIndex(
    const std::string &token,
    int vertexCount
)
{
    const std::string indexText = firstFaceTokenPart(token);
    int objIndex = 0;

    if (indexText.empty())
        return std::nullopt;

    const auto result = std::from_chars(
        indexText.data(),
        indexText.data() + indexText.size(),
        objIndex
    );

    if (result.ec != std::errc{} ||
        result.ptr != indexText.data() + indexText.size())
        return std::nullopt;

    if (objIndex > 0)
        --objIndex;
    else if (objIndex < 0)
        objIndex = vertexCount + objIndex;
    else
        return std::nullopt;

    if (objIndex < 0 || objIndex >= vertexCount)
        return std::nullopt;

    return objIndex;
}

std::string ObjModelLoader3D::parentDirectory(const std::string &path)
{
    const std::size_t slash = path.find_last_of("/\\");

    if (slash == std::string::npos)
        return ".";
    return path.substr(0, slash);
}

std::string ObjModelLoader3D::materialPath(
    const std::string &objPath,
    const std::string &mtlFile
)
{
    return parentDirectory(objPath) + "/" + mtlFile;
}

std::optional<Model3D> ObjModelLoader3D::buildModel(
    const ObjParseContext &context
)
{
    std::vector<ModelSubMesh3D> subMeshes;

    for (const auto &[materialName, vertices] : context.materialVertices) {
        if (vertices.empty())
            continue;

        Magnum::Color4 color = DefaultColor;
        const auto material = context.materials.find(materialName);

        if (material != context.materials.end())
            color = material->second;

        subMeshes.push_back(ModelSubMesh3D{
            .vertices = vertices,
            .color = color
        });
    }

    if (subMeshes.empty())
        return std::nullopt;

    return Model3D(std::move(subMeshes));
}

}