#include "render/MagnumRenderer.hpp"

#include <Corrade/Containers/ArrayView.h>

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix3.h>

#include <array>
#include <vector>

namespace {

struct Vertex {
    Magnum::Vector2 position;
};

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

void appendFoodMarker(std::vector<Vertex> &vertices, int x, int y)
{
    const float left = static_cast<float>(x) + 0.35f;
    const float right = static_cast<float>(x) + 0.65f;
    const float bottom = static_cast<float>(y) + 0.35f;
    const float top = static_cast<float>(y) + 0.65f;

    vertices.push_back({{left, bottom}});
    vertices.push_back({{right, bottom}});
    vertices.push_back({{right, top}});

    vertices.push_back({{left, bottom}});
    vertices.push_back({{right, top}});
    vertices.push_back({{left, top}});
}

std::vector<Vertex> buildFoodVertices(const GameState &state)
{
    std::vector<Vertex> vertices;

    for (int y = 0; y < state.height(); ++y) {
        for (int x = 0; x < state.width(); ++x) {
            const Tile *tile = state.tileAt(x, y);

            if (tile == nullptr)
                continue;

            const Tile::ResourceArray &resources = tile->resources();

            if (resources[0] > 0)
                appendFoodMarker(vertices, x, y);
        }
    }

    return vertices;
}

Magnum::Matrix3 buildMapProjection(int width, int height)
{
    const float mapWidth = static_cast<float>(width);
    const float mapHeight = static_cast<float>(height);

    return Magnum::Matrix3::projection({mapWidth, mapHeight}) *
           Magnum::Matrix3::translation({-mapWidth / 2.0f, -mapHeight / 2.0f});
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
    }

    swapBuffers();
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
        .setTransformationProjectionMatrix(buildMapProjection(width, height))
        .draw(mesh);
}

void MagnumRenderer::drawTileResources(const GameState &state)
{
    const int width = state.width();
    const int height = state.height();

    if (width <= 0 || height <= 0)
        return;

    const std::vector<Vertex> vertices = buildFoodVertices(state);

    if (vertices.empty())
        return;

    Magnum::GL::Mesh mesh = buildMesh(Magnum::GL::MeshPrimitive::Triangles, vertices);

    _shader
        .setColor(Magnum::Color4{0.35f, 0.95f, 0.35f, 1.0f})
        .setTransformationProjectionMatrix(buildMapProjection(width, height))
        .draw(mesh);
}