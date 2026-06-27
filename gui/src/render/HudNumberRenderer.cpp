#include "render/HudNumberRenderer.hpp"

#include "render/RenderMeshFactory.hpp"

#include <Magnum/GL/Mesh.h>

#include <array>
#include <string>
#include <vector>

namespace zappy::render {
namespace {

constexpr int DigitWidth = 3;
constexpr int DigitHeight = 5;
constexpr float CellSize = 4.0f;
constexpr float CellSpacing = 1.0f;
constexpr float DigitSpacing = 4.0f;

using DigitBitmap = std::array<std::array<int, DigitWidth>, DigitHeight>;

const std::array<DigitBitmap, 10> Digits = {{
    {{{1,1,1}, {1,0,1}, {1,0,1}, {1,0,1}, {1,1,1}}}, // 0
    {{{0,1,0}, {1,1,0}, {0,1,0}, {0,1,0}, {1,1,1}}}, // 1
    {{{1,1,1}, {0,0,1}, {1,1,1}, {1,0,0}, {1,1,1}}}, // 2
    {{{1,1,1}, {0,0,1}, {1,1,1}, {0,0,1}, {1,1,1}}}, // 3
    {{{1,0,1}, {1,0,1}, {1,1,1}, {0,0,1}, {0,0,1}}}, // 4
    {{{1,1,1}, {1,0,0}, {1,1,1}, {0,0,1}, {1,1,1}}}, // 5
    {{{1,1,1}, {1,0,0}, {1,1,1}, {1,0,1}, {1,1,1}}}, // 6
    {{{1,1,1}, {0,0,1}, {0,0,1}, {0,0,1}, {0,0,1}}}, // 7
    {{{1,1,1}, {1,0,1}, {1,1,1}, {1,0,1}, {1,1,1}}}, // 8
    {{{1,1,1}, {1,0,1}, {1,1,1}, {0,0,1}, {1,1,1}}}  // 9
}};

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

}

HudNumberRenderer::HudNumberRenderer(Magnum::Shaders::FlatGL2D &shader)
    : _shader(shader)
{
}

void HudNumberRenderer::drawDigit(
    const Magnum::Matrix3 &projection,
    const Magnum::Vector2 &position,
    int digit,
    float scale,
    const Magnum::Color4 &color
)
{
    if (digit < 0 || digit > 9)
        return;

    std::vector<Vertex> vertices;

    const float pixel = CellSize * scale;
    const float spacing = CellSpacing * scale;

    for (int row = 0; row < DigitHeight; ++row) {
        for (int col = 0; col < DigitWidth; ++col) {
            if (Digits[digit][row][col] == 0)
                continue;

            const Magnum::Vector2 cellPosition{
                position.x() + static_cast<float>(col) * (pixel + spacing),
                position.y() + static_cast<float>(DigitHeight - 1 - row) * (pixel + spacing)
            };

            addQuad(vertices, cellPosition, Magnum::Vector2{pixel, pixel});
        }
    }

    if (vertices.empty())
        return;

    Magnum::GL::Mesh mesh = RenderMeshFactory::build(
        Magnum::GL::MeshPrimitive::Triangles,
        vertices
    );

    _shader
        .setTransformationProjectionMatrix(projection)
        .setColor(color)
        .draw(mesh);
}

void HudNumberRenderer::drawNumber(
    const Magnum::Matrix3 &projection,
    const Magnum::Vector2 &position,
    int value,
    float scale,
    const Magnum::Color4 &color
)
{
    const std::string text = std::to_string(value);

    const float digitAdvance =
        static_cast<float>(DigitWidth) * (CellSize * scale + CellSpacing * scale)
        + DigitSpacing * scale;

    for (std::size_t i = 0; i < text.size(); ++i) {
        const int digit = text[i] - '0';

        drawDigit(
            projection,
            position + Magnum::Vector2{static_cast<float>(i) * digitAdvance, 0.0f},
            digit,
            scale,
            color
        );
    }
}

}