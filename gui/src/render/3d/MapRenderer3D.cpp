#include "render/3d/MapRenderer3D.hpp"

#include "render/3d/RenderGeometry3D.hpp"
#include "render/3d/RenderMeshFactory3D.hpp"

#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

namespace zappy::render3d {
namespace {

constexpr float Pi = 3.14159265358979323846f;
constexpr float GridLift = 0.025f;

constexpr std::size_t SurfaceChunkSize = 6;
constexpr std::size_t GridChunkSize = 2;

constexpr Magnum::Color4 PlanetSurfaceColor{0.08f, 0.10f, 0.15f, 1.0f};
constexpr Magnum::Color4 PlanetGridColor{0.85f, 0.90f, 1.0f, 1.0f};

float planetRadius(int width, int height)
{
    const float mapSize = static_cast<float>(std::max(width, height));

    return mapSize * 0.45f + 2.0f;
}

void addLine(
    std::vector<Vertex3D> &vertices,
    const Magnum::Vector3 &from,
    const Magnum::Vector3 &to
)
{
    vertices.push_back({from});
    vertices.push_back({to});
}

void addTriangle(
    std::vector<Vertex3D> &vertices,
    const Magnum::Vector3 &a,
    const Magnum::Vector3 &b,
    const Magnum::Vector3 &c
)
{
    vertices.push_back({a});
    vertices.push_back({b});
    vertices.push_back({c});
}

void addQuad(
    std::vector<Vertex3D> &vertices,
    const Magnum::Vector3 &a,
    const Magnum::Vector3 &b,
    const Magnum::Vector3 &c,
    const Magnum::Vector3 &d
)
{
    addTriangle(vertices, a, b, c);
    addTriangle(vertices, a, c, d);
}

class PlanetGridBuilder {
public:
    PlanetGridBuilder(int width, int height)
        : _width(width),
          _height(height),
          _radius(planetRadius(width, height))
    {
    }

    bool isValid() const
    {
        return _width > 0 && _height > 0;
    }

    std::vector<Vertex3D> buildSurface() const
    {
        std::vector<Vertex3D> vertices;

        if (!isValid())
            return vertices;

        vertices.reserve(static_cast<std::size_t>(_width * _height) * SurfaceChunkSize);

        for (int y = 0; y < _height; ++y) {
            for (int x = 0; x < _width; ++x)
                addTileSurface(vertices, x, y);
        }

        return vertices;
    }

    std::vector<Vertex3D> buildGrid() const
    {
        std::vector<Vertex3D> vertices;

        if (!isValid())
            return vertices;

        vertices.reserve(gridVertexCount());

        addVerticalGridLines(vertices);
        addHorizontalGridLines(vertices);

        return vertices;
    }

private:
    std::size_t gridVertexCount() const
    {
        const std::size_t verticalLines =
            static_cast<std::size_t>(_width * _height);

        const std::size_t horizontalLines =
            static_cast<std::size_t>((_height + 1) * _width);

        return (verticalLines + horizontalLines) * GridChunkSize;
    }

    float longitude(int x) const
    {
        return (static_cast<float>(x) / static_cast<float>(_width)) * 2.0f * Pi;
    }

    float latitude(int y) const
    {
        return -Pi * 0.5f
            + (static_cast<float>(y) / static_cast<float>(_height)) * Pi;
    }

    Magnum::Vector3 spherePoint(float radius, float longitude, float latitude) const
    {
        const float cosLatitude = std::cos(latitude);

        return Magnum::Vector3{
            radius * cosLatitude * std::cos(longitude),
            radius * std::sin(latitude),
            radius * cosLatitude * std::sin(longitude)
        };
    }

    Magnum::Vector3 mapPoint(int x, int y, float lift = 0.0f) const
    {
        return spherePoint(
            _radius + lift,
            longitude(x),
            latitude(y)
        );
    }

    void addTileSurface(
        std::vector<Vertex3D> &vertices,
        int x,
        int y
    ) const
    {
        const Magnum::Vector3 a = mapPoint(x, y);
        const Magnum::Vector3 b = mapPoint(x + 1, y);
        const Magnum::Vector3 c = mapPoint(x + 1, y + 1);
        const Magnum::Vector3 d = mapPoint(x, y + 1);

        addQuad(vertices, a, b, c, d);
    }

    void addVerticalGridLines(std::vector<Vertex3D> &vertices) const
    {
        for (int x = 0; x < _width; ++x) {
            for (int y = 0; y < _height; ++y) {
                addLine(
                    vertices,
                    mapPoint(x, y, GridLift),
                    mapPoint(x, y + 1, GridLift)
                );
            }
        }
    }

    void addHorizontalGridLines(std::vector<Vertex3D> &vertices) const
    {
        for (int y = 0; y <= _height; ++y) {
            for (int x = 0; x < _width; ++x) {
                addLine(
                    vertices,
                    mapPoint(x, y, GridLift),
                    mapPoint(x + 1, y, GridLift)
                );
            }
        }
    }

    int _width;
    int _height;
    float _radius;
};

std::vector<Vertex3D> visibleChunkVertices(
    const std::vector<Vertex3D> &vertices,
    std::size_t chunkSize,
    float progress
)
{
    if (vertices.empty() || chunkSize == 0)
        return {};

    const float clampedProgress = std::clamp(progress, 0.0f, 1.0f);
    const std::size_t chunkCount = vertices.size() / chunkSize;
    const std::size_t visibleChunks = static_cast<std::size_t>(
        static_cast<float>(chunkCount) * clampedProgress
    );

    const std::size_t visibleVertexCount = std::min(
        vertices.size(),
        visibleChunks * chunkSize
    );

    return std::vector<Vertex3D>(
        vertices.begin(),
        vertices.begin() + static_cast<std::ptrdiff_t>(visibleVertexCount)
    );
}

void drawMesh(
    Magnum::Shaders::FlatGL3D &shader,
    const Magnum::Matrix4 &projection,
    MeshPrimitive3D primitive,
    const std::vector<Vertex3D> &vertices,
    const Magnum::Color4 &color
)
{
    if (vertices.empty())
        return;

    Magnum::GL::Mesh mesh = RenderMeshFactory3D::build(primitive, vertices);

    shader
        .setTransformationProjectionMatrix(projection)
        .setColor(color)
        .draw(mesh);
}

}

MapRenderer3D::MapRenderer3D(Magnum::Shaders::FlatGL3D &shader)
    : _shader(shader)
{
}

void MapRenderer3D::draw(
    const GameState &state,
    const Magnum::Matrix4 &projection,
    float introProgress
)
{
    const PlanetGridBuilder builder(state.width(), state.height());

    if (!builder.isValid())
        return;

    const std::vector<Vertex3D> surfaceVertices = visibleChunkVertices(
        builder.buildSurface(),
        SurfaceChunkSize,
        introProgress
    );

    const std::vector<Vertex3D> gridVertices = visibleChunkVertices(
        builder.buildGrid(),
        GridChunkSize,
        introProgress
    );

    drawMesh(
        _shader,
        projection,
        MeshPrimitive3D::Triangles,
        surfaceVertices,
        PlanetSurfaceColor
    );

    drawMesh(
        _shader,
        projection,
        MeshPrimitive3D::Lines,
        gridVertices,
        PlanetGridColor
    );
}

}