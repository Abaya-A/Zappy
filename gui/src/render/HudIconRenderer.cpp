#include "render/HudIconRenderer.hpp"

#include "render/RenderGeometry.hpp"
#include "render/RenderMeshFactory.hpp"

#include <Magnum/GL/Mesh.h>

#include <vector>

namespace zappy::render {
namespace {

void addQuad(
    std::vector<Vertex> &vertices,
    const Magnum::Vector2 &position,
    const Magnum::Vector2 &size
)
{
    const Magnum::Vector2 a = position;
    const Magnum::Vector2 b = position + Magnum::Vector2{size.x(), 0.0f};
    const Magnum::Vector2 c = position + size;
    const Magnum::Vector2 d = position + Magnum::Vector2{0.0f, size.y()};

    vertices.push_back({a});
    vertices.push_back({b});
    vertices.push_back({c});

    vertices.push_back({a});
    vertices.push_back({c});
    vertices.push_back({d});
}

void drawQuads(
    Magnum::Shaders::FlatGL2D &shader,
    const Magnum::Matrix3 &projection,
    const std::vector<Vertex> &vertices,
    const Magnum::Color4 &color
)
{
    if (vertices.empty())
        return;

    Magnum::GL::Mesh mesh = RenderMeshFactory::build(
        Magnum::GL::MeshPrimitive::Triangles,
        vertices
    );

    shader
        .setTransformationProjectionMatrix(projection)
        .setColor(color)
        .draw(mesh);
}

Magnum::Color4 lighter(const Magnum::Color4 &color)
{
    return {
        0.55f + color.r() * 0.45f,
        0.55f + color.g() * 0.45f,
        0.55f + color.b() * 0.45f,
        color.a()
    };
}

}

HudIconRenderer::HudIconRenderer(Magnum::Shaders::FlatGL2D &shader)
    : _shader(shader)
{
}

void HudIconRenderer::drawPlayerIcon(
    const Magnum::Matrix3 &projection,
    const Magnum::Vector2 &position,
    float scale,
    const Magnum::Color4 &color
)
{
    const float unit = 4.0f * scale;

    std::vector<Vertex> shadow;
    std::vector<Vertex> body;
    std::vector<Vertex> head;

    shadow.reserve(6);
    body.reserve(30);
    head.reserve(6);

    addQuad(
        shadow,
        position + Magnum::Vector2{unit * 0.5f, 0.0f},
        Magnum::Vector2{unit * 3.0f, unit * 0.55f}
    );

    addQuad(
        body,
        position + Magnum::Vector2{unit * 1.0f, unit * 0.7f},
        Magnum::Vector2{unit * 2.0f, unit * 2.0f}
    );

    addQuad(
        body,
        position + Magnum::Vector2{unit * 0.4f, unit * 1.2f},
        Magnum::Vector2{unit * 0.8f, unit * 1.4f}
    );

    addQuad(
        body,
        position + Magnum::Vector2{unit * 2.8f, unit * 1.2f},
        Magnum::Vector2{unit * 0.8f, unit * 1.4f}
    );

    addQuad(
        body,
        position + Magnum::Vector2{unit * 1.0f, unit * 0.2f},
        Magnum::Vector2{unit * 0.75f, unit * 0.8f}
    );

    addQuad(
        body,
        position + Magnum::Vector2{unit * 2.25f, unit * 0.2f},
        Magnum::Vector2{unit * 0.75f, unit * 0.8f}
    );

    addQuad(
        head,
        position + Magnum::Vector2{unit * 1.25f, unit * 3.0f},
        Magnum::Vector2{unit * 1.5f, unit * 1.5f}
    );

    drawQuads(
        _shader,
        projection,
        shadow,
        Magnum::Color4{0.0f, 0.0f, 0.0f, 0.35f}
    );

    drawQuads(
        _shader,
        projection,
        body,
        color
    );

    drawQuads(
        _shader,
        projection,
        head,
        lighter(color)
    );
}

void HudIconRenderer::drawEggIcon(
    const Magnum::Matrix3 &projection,
    const Magnum::Vector2 &position,
    float scale,
    const Magnum::Color4 &color
)
{
    const float unit = 4.0f * scale;

    std::vector<Vertex> shadow;
    std::vector<Vertex> egg;
    std::vector<Vertex> shine;

    shadow.reserve(6);
    egg.reserve(36);
    shine.reserve(6);

    addQuad(
        shadow,
        position + Magnum::Vector2{unit * 0.4f, 0.0f},
        Magnum::Vector2{unit * 2.7f, unit * 0.5f}
    );

    addQuad(
        egg,
        position + Magnum::Vector2{unit * 1.0f, unit * 0.5f},
        Magnum::Vector2{unit * 1.5f, unit * 0.5f}
    );

    addQuad(
        egg,
        position + Magnum::Vector2{unit * 0.6f, unit * 1.0f},
        Magnum::Vector2{unit * 2.3f, unit * 0.8f}
    );

    addQuad(
        egg,
        position + Magnum::Vector2{unit * 0.4f, unit * 1.8f},
        Magnum::Vector2{unit * 2.7f, unit * 1.2f}
    );

    addQuad(
        egg,
        position + Magnum::Vector2{unit * 0.8f, unit * 3.0f},
        Magnum::Vector2{unit * 1.9f, unit * 0.8f}
    );

    addQuad(
        egg,
        position + Magnum::Vector2{unit * 1.2f, unit * 3.8f},
        Magnum::Vector2{unit * 1.1f, unit * 0.5f}
    );

    addQuad(
        shine,
        position + Magnum::Vector2{unit * 1.4f, unit * 2.9f},
        Magnum::Vector2{unit * 0.45f, unit * 0.45f}
    );

    drawQuads(
        _shader,
        projection,
        shadow,
        Magnum::Color4{0.0f, 0.0f, 0.0f, 0.30f}
    );

    drawQuads(
        _shader,
        projection,
        egg,
        color
    );

    drawQuads(
        _shader,
        projection,
        shine,
        lighter(color)
    );
}

}