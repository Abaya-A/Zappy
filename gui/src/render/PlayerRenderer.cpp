#include "render/PlayerRenderer.hpp"

#include "render/RenderGeometry.hpp"
#include "render/RenderMeshFactory.hpp"

#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>

#include <string>
#include <vector>

namespace {

Magnum::Color4 colorForTeam(const std::string &teamName)
{
    if (teamName == "teamA")
        return Magnum::Color4{0.20f, 0.45f, 1.00f, 1.0f};

    if (teamName == "teamB")
        return Magnum::Color4{1.00f, 0.55f, 0.20f, 1.0f};

    return Magnum::Color4{0.75f, 0.75f, 0.95f, 1.0f};
}

float levelScale(int level)
{
    if (level <= 1)
        return 1.0f;

    if (level >= 8)
        return 1.7f;

    return 1.0f + static_cast<float>(level - 1) * 0.10f;
}

void appendPlayerTriangle(
    std::vector<zappy::render::Vertex> &vertices,
    int x,
    int y,
    int orientation,
    int level
)
{
    const float centerX = static_cast<float>(x) + 0.5f;
    const float centerY = static_cast<float>(y) + 0.5f;
    const float scale = levelScale(level);

    const float front = 0.24f * scale;
    const float back = 0.16f * scale;
    const float halfWidth = 0.20f * scale;

    switch (orientation) {
        case 1:
            vertices.push_back({{centerX, centerY + front}});
            vertices.push_back({{centerX - halfWidth, centerY - back}});
            vertices.push_back({{centerX + halfWidth, centerY - back}});
            break;
        case 2:
            vertices.push_back({{centerX + front, centerY}});
            vertices.push_back({{centerX - back, centerY - halfWidth}});
            vertices.push_back({{centerX - back, centerY + halfWidth}});
            break;
        case 3:
            vertices.push_back({{centerX, centerY - front}});
            vertices.push_back({{centerX + halfWidth, centerY + back}});
            vertices.push_back({{centerX - halfWidth, centerY + back}});
            break;
        case 4:
            vertices.push_back({{centerX - front, centerY}});
            vertices.push_back({{centerX + back, centerY + halfWidth}});
            vertices.push_back({{centerX + back, centerY - halfWidth}});
            break;
        default:
            vertices.push_back({{centerX, centerY + front}});
            vertices.push_back({{centerX - halfWidth, centerY - back}});
            vertices.push_back({{centerX + halfWidth, centerY - back}});
            break;
    }
}

} // namespace

namespace zappy::render {

PlayerRenderer::PlayerRenderer(Magnum::Shaders::FlatGL2D &shader)
    : _shader(shader)
{
}

void PlayerRenderer::draw(const GameState &state, const Magnum::Matrix3 &projection)
{
    if (state.width() <= 0 || state.height() <= 0)
        return;

    for (const auto &[id, player] : state.players()) {
        (void)id;

        std::vector<Vertex> vertices;

        appendPlayerTriangle(
            vertices,
            player.x(),
            player.y(),
            player.orientation(),
            player.level()
        );

        Magnum::GL::Mesh mesh =
            RenderMeshFactory::build(Magnum::GL::MeshPrimitive::Triangles, vertices);

        _shader
            .setColor(colorForTeam(player.teamName()))
            .setTransformationProjectionMatrix(projection)
            .draw(mesh);
    }
}

}