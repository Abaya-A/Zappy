#include "render/MagnumRenderer.hpp"

#include <Corrade/Containers/ArrayView.h>

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix3.h>

#include <array>
#include <cstddef>
#include <vector>

namespace {

struct Vertex {
    Magnum::Vector2 position;
};

constexpr int ResourceCount = 7;

struct ResourceMarkerStyle {
    Magnum::Color4 color;
    float offsetX;
    float offsetY;
};

const std::array<ResourceMarkerStyle, ResourceCount> ResourceMarkerStyles = {{
    {Magnum::Color4{0.35f, 0.95f, 0.35f, 1.0f}, 0.18f, 0.18f}, // food
    {Magnum::Color4{0.40f, 0.75f, 1.00f, 1.0f}, 0.42f, 0.18f}, // linemate
    {Magnum::Color4{0.85f, 0.65f, 1.00f, 1.0f}, 0.66f, 0.18f}, // deraumere
    {Magnum::Color4{1.00f, 0.85f, 0.35f, 1.0f}, 0.18f, 0.42f}, // sibur
    {Magnum::Color4{1.00f, 0.55f, 0.35f, 1.0f}, 0.42f, 0.42f}, // mendiane
    {Magnum::Color4{0.95f, 0.35f, 0.75f, 1.0f}, 0.66f, 0.42f}, // phiras
    {Magnum::Color4{0.90f, 0.90f, 0.95f, 1.0f}, 0.42f, 0.66f}, // thystame
}};

std::vector<Vertex> buildGridVertices(int width, int height)
{
    std::vector<Vertex> vertices;

    vertices.reserve(static_cast<std::size_t>((width + height + 2) * 2));

    for (int x = 0; x <= width; ++x) {
        const float xf = static_cast<float>(x);

        vertices.push_back({{xf, 0.0f}});
        vertices.push_back({{xf, static_cast<float>(height)}});
    }

    for (int y = 0; y <= height; ++y) {
        const float yf = static_cast<float>(y);

        vertices.push_back({{0.0f, yf}});
        vertices.push_back({{static_cast<float>(width), yf}});
    }

    return vertices;
}

void appendSquareMarker(
    std::vector<Vertex> &vertices,
    int tileX,
    int tileY,
    float offsetX,
    float offsetY
)
{
    constexpr float MarkerSize = 0.16f;

    const float left = static_cast<float>(tileX) + offsetX;
    const float right = left + MarkerSize;
    const float bottom = static_cast<float>(tileY) + offsetY;
    const float top = bottom + MarkerSize;

    vertices.push_back({{left, bottom}});
    vertices.push_back({{right, bottom}});
    vertices.push_back({{right, top}});

    vertices.push_back({{left, bottom}});
    vertices.push_back({{right, top}});
    vertices.push_back({{left, top}});
}

std::array<std::vector<Vertex>, ResourceCount> buildResourceVertices(const GameState &state)
{
    std::array<std::vector<Vertex>, ResourceCount> verticesByResource;

    for (int y = 0; y < state.height(); ++y) {
        for (int x = 0; x < state.width(); ++x) {
            const Tile *tile = state.tileAt(x, y);

            if (tile == nullptr)
                continue;

            const Tile::ResourceArray &resources = tile->resources();

            for (int resourceId = 0; resourceId < ResourceCount; ++resourceId) {
                if (resources[resourceId] <= 0)
                    continue;

                const ResourceMarkerStyle &style = ResourceMarkerStyles[resourceId];

                appendSquareMarker(
                    verticesByResource[resourceId],
                    x,
                    y,
                    style.offsetX,
                    style.offsetY
                );
            }
        }
    }

    return verticesByResource;
}

void appendPlayerTriangle(
    std::vector<Vertex> &vertices,
    int x,
    int y,
    int orientation
)
{
    const float centerX = static_cast<float>(x) + 0.5f;
    const float centerY = static_cast<float>(y) + 0.5f;

    constexpr float Front = 0.32f;
    constexpr float Back = 0.22f;
    constexpr float HalfWidth = 0.22f;

    switch (orientation) {
        case 1: // north
            vertices.push_back({{centerX, centerY + Front}});
            vertices.push_back({{centerX - HalfWidth, centerY - Back}});
            vertices.push_back({{centerX + HalfWidth, centerY - Back}});
            break;
        case 2: // east
            vertices.push_back({{centerX + Front, centerY}});
            vertices.push_back({{centerX - Back, centerY - HalfWidth}});
            vertices.push_back({{centerX - Back, centerY + HalfWidth}});
            break;
        case 3: // south
            vertices.push_back({{centerX, centerY - Front}});
            vertices.push_back({{centerX + HalfWidth, centerY + Back}});
            vertices.push_back({{centerX - HalfWidth, centerY + Back}});
            break;
        case 4: // west
            vertices.push_back({{centerX - Front, centerY}});
            vertices.push_back({{centerX + Back, centerY + HalfWidth}});
            vertices.push_back({{centerX + Back, centerY - HalfWidth}});
            break;
        default:
            vertices.push_back({{centerX, centerY + Front}});
            vertices.push_back({{centerX - HalfWidth, centerY - Back}});
            vertices.push_back({{centerX + HalfWidth, centerY - Back}});
            break;
    }
}

std::vector<Vertex> buildPlayerVertices(const GameState &state)
{
    std::vector<Vertex> vertices;

    for (const auto &[id, player] : state.players()) {
        (void)id;

        appendPlayerTriangle(
            vertices,
            player.x(),
            player.y(),
            player.orientation()
        );
    }

    return vertices;
}

void appendEggDiamond(std::vector<Vertex> &vertices, int x, int y)
{
    const float centerX = static_cast<float>(x) + 0.5f;
    const float centerY = static_cast<float>(y) + 0.5f;

    constexpr float Radius = 0.18f;

    const Magnum::Vector2 top{centerX, centerY + Radius};
    const Magnum::Vector2 right{centerX + Radius, centerY};
    const Magnum::Vector2 bottom{centerX, centerY - Radius};
    const Magnum::Vector2 left{centerX - Radius, centerY};

    vertices.push_back({top});
    vertices.push_back({right});
    vertices.push_back({bottom});

    vertices.push_back({top});
    vertices.push_back({bottom});
    vertices.push_back({left});
}

std::vector<Vertex> buildEggVertices(const GameState &state)
{
    std::vector<Vertex> vertices;

    for (const auto &[id, egg] : state.eggs()) {
        (void)id;
        appendEggDiamond(vertices, egg.x(), egg.y());
    }

    return vertices;
}

void uploadVertices(Magnum::GL::Buffer &buffer, const std::vector<Vertex> &vertices)
{
    buffer.setData(
        Corrade::Containers::arrayCast<const char>(
            Corrade::Containers::arrayView(vertices.data(), vertices.size())
        )
    );
}

Magnum::GL::Mesh buildMesh(
    Magnum::GL::MeshPrimitive primitive,
    const std::vector<Vertex> &vertices
)
{
    Magnum::GL::Buffer vertexBuffer;
    uploadVertices(vertexBuffer, vertices);

    Magnum::GL::Mesh mesh;
    mesh.setPrimitive(primitive)
        .setCount(static_cast<int>(vertices.size()))
        .addVertexBuffer(
            std::move(vertexBuffer),
            0,
            Magnum::Shaders::FlatGL2D::Position{}
        );

    return mesh;
}

} // namespace

MagnumRenderer::MagnumRenderer(const Arguments &arguments)
    : Magnum::Platform::Sdl2Application(
          arguments,
          Configuration{}
              .setTitle("Zappy GUI")
              .setSize({1280, 720})
      ),
      _state(nullptr),
      _isOpen(true),
      _shader()
{
    Magnum::GL::Renderer::setClearColor(Magnum::Color4{0.06f, 0.06f, 0.09f, 1.0f});
}

bool MagnumRenderer::isOpen() const
{
    return _isOpen;
}

void MagnumRenderer::handleEvents()
{
    mainLoopIteration();
}

void MagnumRenderer::render(const GameState &state)
{
    _state = &state;
    redraw();
}

void MagnumRenderer::drawEvent()
{
    Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);

    if (_state != nullptr && _state->isReady()) {
        drawMapGrid(*_state);
        drawTileResources(*_state);
        drawEggs(*_state);
        drawPlayers(*_state);
    }

    swapBuffers();
}

void MagnumRenderer::viewportEvent(ViewportEvent &event)
{
    Magnum::GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
}

Magnum::Matrix3 MagnumRenderer::buildCameraProjection(int width, int height) const
{
    const float mapWidth = static_cast<float>(width);
    const float mapHeight = static_cast<float>(height);

    if (mapWidth <= 0.0f || mapHeight <= 0.0f)
        return Magnum::Matrix3::projection({1.0f, 1.0f});

    const Magnum::Vector2i framebuffer = framebufferSize();

    if (framebuffer.x() <= 0 || framebuffer.y() <= 0)
        return Magnum::Matrix3::projection({mapWidth, mapHeight});

    const float framebufferAspect =
        static_cast<float>(framebuffer.x()) / static_cast<float>(framebuffer.y());
    const float mapAspect = mapWidth / mapHeight;

    float viewWidth = mapWidth;
    float viewHeight = mapHeight;

    if (framebufferAspect > mapAspect) {
        viewHeight = mapHeight;
        viewWidth = mapHeight * framebufferAspect;
    } else {
        viewWidth = mapWidth;
        viewHeight = mapWidth / framebufferAspect;
    }

    return Magnum::Matrix3::projection({viewWidth, viewHeight}) *
           Magnum::Matrix3::translation({-mapWidth / 2.0f, -mapHeight / 2.0f});
}

void MagnumRenderer::drawMapGrid(const GameState &state)
{
    const int width = state.width();
    const int height = state.height();

    if (width <= 0 || height <= 0)
        return;

    const std::vector<Vertex> vertices = buildGridVertices(width, height);
    Magnum::GL::Mesh mesh = buildMesh(Magnum::GL::MeshPrimitive::Lines, vertices);

    _shader
        .setColor(Magnum::Color4{0.75f, 0.75f, 0.85f, 1.0f})
        .setTransformationProjectionMatrix(buildCameraProjection(width, height))
        .draw(mesh);
}

void MagnumRenderer::drawTileResources(const GameState &state)
{
    const int width = state.width();
    const int height = state.height();

    if (width <= 0 || height <= 0)
        return;

    const std::array<std::vector<Vertex>, ResourceCount> verticesByResource =
        buildResourceVertices(state);

    for (int resourceId = 0; resourceId < ResourceCount; ++resourceId) {
        const std::vector<Vertex> &vertices = verticesByResource[resourceId];

        if (vertices.empty())
            continue;

        Magnum::GL::Mesh mesh = buildMesh(Magnum::GL::MeshPrimitive::Triangles, vertices);

        _shader
            .setColor(ResourceMarkerStyles[resourceId].color)
            .setTransformationProjectionMatrix(buildCameraProjection(width, height))
            .draw(mesh);
    }
}

void MagnumRenderer::drawEggs(const GameState &state)
{
    const int width = state.width();
    const int height = state.height();

    if (width <= 0 || height <= 0)
        return;

    const std::vector<Vertex> vertices = buildEggVertices(state);

    if (vertices.empty())
        return;

    Magnum::GL::Mesh mesh = buildMesh(Magnum::GL::MeshPrimitive::Triangles, vertices);

    _shader
        .setColor(Magnum::Color4{1.0f, 1.0f, 0.85f, 1.0f})
        .setTransformationProjectionMatrix(buildCameraProjection(width, height))
        .draw(mesh);
}

void MagnumRenderer::drawPlayers(const GameState &state)
{
    const int width = state.width();
    const int height = state.height();

    if (width <= 0 || height <= 0)
        return;

    const std::vector<Vertex> vertices = buildPlayerVertices(state);

    if (vertices.empty())
        return;

    Magnum::GL::Mesh mesh = buildMesh(Magnum::GL::MeshPrimitive::Triangles, vertices);

    _shader
        .setColor(Magnum::Color4{0.20f, 0.45f, 1.00f, 1.0f})
        .setTransformationProjectionMatrix(buildCameraProjection(width, height))
        .draw(mesh);
}