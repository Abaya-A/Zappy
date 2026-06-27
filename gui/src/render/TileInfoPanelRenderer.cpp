#include "render/TileInfoPanelRenderer.hpp"

#include "render/RenderGeometry.hpp"
#include "render/RenderMeshFactory.hpp"
#include "state/Resource.hpp"
#include "state/Tile.hpp"

#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>

#include <algorithm>
#include <vector>

namespace zappy::render {
namespace {

constexpr float PanelWidth = 250.0f;
constexpr float PanelHeight = 300.0f;
constexpr float PanelMargin = 25.0f;
constexpr float PanelPadding = 14.0f;

constexpr float HeaderHeight = 20.0f;
constexpr float ResourceRowHeight = 30.0f;
constexpr float ResourceBarHeight = 8.0f;
constexpr float ResourceBarMaxWidth = 145.0f;
constexpr float ResourceBarStartX = 78.0f;

constexpr float CloseButtonSize = 18.0f;
constexpr float CloseButtonMargin = 6.0f;
constexpr float CloseIconThickness = 2.0f;

constexpr float MarkerSize = 8.0f;
constexpr int MaxDisplayedQuantity = 10;

struct PanelGeometry {
    Magnum::Vector2 origin;
    Magnum::Vector2 size;
};

struct TileSummary {
    Tile::ResourceArray resources;
    int playerCount;
    int eggCount;
};

const std::array<Magnum::Color4, Resource::COUNT> ResourceColors = {
    Magnum::Color4{0.95f, 0.85f, 0.35f, 1.0f},
    Magnum::Color4{0.40f, 0.85f, 1.00f, 1.0f},
    Magnum::Color4{0.55f, 1.00f, 0.55f, 1.0f},
    Magnum::Color4{0.90f, 0.55f, 1.00f, 1.0f},
    Magnum::Color4{1.00f, 0.55f, 0.50f, 1.0f},
    Magnum::Color4{0.55f, 0.65f, 1.00f, 1.0f},
    Magnum::Color4{1.00f, 1.00f, 1.00f, 1.0f}
};

PanelGeometry panelGeometry(const Magnum::Vector2i &framebufferSize)
{
    const Magnum::Vector2 size{PanelWidth, PanelHeight};

    return {
        Magnum::Vector2{
            static_cast<float>(framebufferSize.x()) - PanelWidth - PanelMargin,
            PanelMargin
        },
        size
    };
}

Magnum::Matrix3 screenProjection(const Magnum::Vector2i &framebufferSize)
{
    return Magnum::Matrix3::projection({
        static_cast<float>(framebufferSize.x()),
        static_cast<float>(framebufferSize.y())
    }) * Magnum::Matrix3::translation({
        -static_cast<float>(framebufferSize.x()) * 0.5f,
        -static_cast<float>(framebufferSize.y()) * 0.5f
    });
}

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

void drawQuad(
    Magnum::Shaders::FlatGL2D &shader,
    const Magnum::Matrix3 &projection,
    const Magnum::Vector2 &position,
    const Magnum::Vector2 &size,
    const Magnum::Color4 &color
)
{
    std::vector<Vertex> vertices;
    vertices.reserve(6);

    addQuad(vertices, position, size);

    Magnum::GL::Mesh mesh = RenderMeshFactory::build(
        Magnum::GL::MeshPrimitive::Triangles,
        vertices
    );

    shader
        .setTransformationProjectionMatrix(projection)
        .setColor(color)
        .draw(mesh);
}

TileSummary summarizeTile(
    const GameState &state,
    const Magnum::Vector2i &selectedTile
)
{
    TileSummary summary{
        .resources = {},
        .playerCount = 0,
        .eggCount = 0
    };

    const Tile *tile = state.tileAt(selectedTile.x(), selectedTile.y());

    if (tile != nullptr)
        summary.resources = tile->resources();

    for (const auto &[id, player] : state.players()) {
        (void)id;
        if (player.x() == selectedTile.x() && player.y() == selectedTile.y())
            ++summary.playerCount;
    }

    for (const auto &[id, egg] : state.eggs()) {
        (void)id;
        if (egg.x() == selectedTile.x() && egg.y() == selectedTile.y())
            ++summary.eggCount;
    }

    return summary;
}

Magnum::Vector2 closeButtonPosition(const PanelGeometry &panel)
{
    constexpr float CloseButtonYOffset = 6.0f;

    return {
        panel.origin.x() + panel.size.x() - CloseButtonSize - CloseButtonMargin,
        panel.origin.y() + panel.size.y() - CloseButtonSize - CloseButtonMargin + CloseButtonYOffset
    };
}

bool containsPoint(
    const Magnum::Vector2 &position,
    const Magnum::Vector2 &size,
    const Magnum::Vector2i &point
)
{
    return static_cast<float>(point.x()) >= position.x() &&
           static_cast<float>(point.y()) >= position.y() &&
           static_cast<float>(point.x()) < position.x() + size.x() &&
           static_cast<float>(point.y()) < position.y() + size.y();
}

void addLineQuad(
    std::vector<Vertex> &vertices,
    const Magnum::Vector2 &start,
    const Magnum::Vector2 &end,
    float thickness
)
{
    const Magnum::Vector2 direction = end - start;

    if (direction.length() <= 0.001f)
        return;

    const Magnum::Vector2 normal =
        Magnum::Vector2{-direction.y(), direction.x()}.normalized() *
        (thickness * 0.5f);

    const Magnum::Vector2 a = start + normal;
    const Magnum::Vector2 b = end + normal;
    const Magnum::Vector2 c = end - normal;
    const Magnum::Vector2 d = start - normal;

    vertices.push_back({a});
    vertices.push_back({b});
    vertices.push_back({c});

    vertices.push_back({a});
    vertices.push_back({c});
    vertices.push_back({d});
}

void drawLine(
    Magnum::Shaders::FlatGL2D &shader,
    const Magnum::Matrix3 &projection,
    const Magnum::Vector2 &start,
    const Magnum::Vector2 &end,
    float thickness,
    const Magnum::Color4 &color
)
{
    std::vector<Vertex> vertices;
    vertices.reserve(6);

    addLineQuad(vertices, start, end, thickness);

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

void drawCloseButton(
    Magnum::Shaders::FlatGL2D &shader,
    const Magnum::Matrix3 &projection,
    const PanelGeometry &panel
)
{
    const Magnum::Vector2 buttonPosition = closeButtonPosition(panel);
    const Magnum::Vector2 buttonSize{CloseButtonSize, CloseButtonSize};

    drawQuad(
        shader,
        projection,
        buttonPosition,
        buttonSize,
        Magnum::Color4{0.18f, 0.07f, 0.09f, 0.95f}
    );

    const float iconInset = 5.0f;

    const Magnum::Vector2 firstStart =
        buttonPosition + Magnum::Vector2{iconInset, iconInset};
    const Magnum::Vector2 firstEnd =
        buttonPosition + buttonSize - Magnum::Vector2{iconInset, iconInset};

    const Magnum::Vector2 secondStart =
        buttonPosition + Magnum::Vector2{CloseButtonSize - iconInset, iconInset};
    const Magnum::Vector2 secondEnd =
        buttonPosition + Magnum::Vector2{iconInset, CloseButtonSize - iconInset};

    drawLine(
        shader,
        projection,
        firstStart,
        firstEnd,
        CloseIconThickness,
        Magnum::Color4{0.95f, 0.95f, 0.98f, 1.0f}
    );

    drawLine(
        shader,
        projection,
        secondStart,
        secondEnd,
        CloseIconThickness,
        Magnum::Color4{0.95f, 0.95f, 0.98f, 1.0f}
    );
}

float quantityRatio(int quantity)
{
    return std::clamp(
        static_cast<float>(quantity) / static_cast<float>(MaxDisplayedQuantity),
        0.0f,
        1.0f
    );
}

void drawResourceRows(
    Magnum::Shaders::FlatGL2D &shader,
    HudNumberRenderer &numberRenderer,
    const Magnum::Matrix3 &projection,
    const PanelGeometry &panel,
    const TileSummary &summary
)
{
    const float rowsStartY =
        panel.origin.y() + PanelHeight - PanelPadding - HeaderHeight - 18.0f;

    for (std::size_t i = 0; i < Resource::COUNT; ++i) {
        const float y = rowsStartY - static_cast<float>(i) * ResourceRowHeight;
        const int quantity = summary.resources[i];

        drawQuad(
            shader,
            projection,
            panel.origin + Magnum::Vector2{PanelPadding, y - panel.origin.y()},
            Magnum::Vector2{MarkerSize, MarkerSize},
            ResourceColors[i]
        );

        drawQuad(
            shader,
            projection,
            panel.origin + Magnum::Vector2{ResourceBarStartX, y - panel.origin.y()},
            Magnum::Vector2{
                ResourceBarMaxWidth,
                ResourceBarHeight
            },
            Magnum::Color4{0.18f, 0.18f, 0.24f, 0.95f}
        );

        drawQuad(
            shader,
            projection,
            panel.origin + Magnum::Vector2{ResourceBarStartX, y - panel.origin.y()},
            Magnum::Vector2{
                ResourceBarMaxWidth * quantityRatio(quantity),
                ResourceBarHeight
            },
            ResourceColors[i]
        );

        numberRenderer.drawNumber(
            projection,
            Magnum::Vector2{
                panel.origin.x() + ResourceBarStartX + ResourceBarMaxWidth + 10.0f,
                y - 2.0f
            },
            quantity,
            0.8f,
            Magnum::Color4{0.92f, 0.92f, 0.96f, 1.0f}
        );
    }
}

void drawEntityRows(
    Magnum::Shaders::FlatGL2D &shader,
    HudNumberRenderer &numberRenderer,
    HudIconRenderer &iconRenderer,
    const Magnum::Matrix3 &projection,
    const PanelGeometry &panel,
    const TileSummary &summary
)
{
    const float playerY = panel.origin.y() + 34.0f;
    const float eggY = panel.origin.y() + 12.0f;

    constexpr int MaxVisibleIcons = 6;
    constexpr float IconScale = 0.75f;
    constexpr float IconAdvance = 20.0f;

    const int visiblePlayers = std::clamp(summary.playerCount, 0, MaxVisibleIcons);
    const int visibleEggs = std::clamp(summary.eggCount, 0, MaxVisibleIcons);

    for (int i = 0; i < visiblePlayers; ++i) {
        iconRenderer.drawPlayerIcon(
            projection,
            Magnum::Vector2{
                panel.origin.x() + PanelPadding + static_cast<float>(i) * IconAdvance,
                playerY
            },
            IconScale,
            Magnum::Color4{0.35f, 0.90f, 1.00f, 1.0f}
        );
    }

    numberRenderer.drawNumber(
        projection,
        Magnum::Vector2{
            panel.origin.x() + ResourceBarStartX + ResourceBarMaxWidth + 10.0f,
            playerY + 2.0f
        },
        summary.playerCount,
        0.75f,
        Magnum::Color4{0.35f, 0.90f, 1.00f, 1.0f}
    );

    for (int i = 0; i < visibleEggs; ++i) {
        iconRenderer.drawEggIcon(
            projection,
            Magnum::Vector2{
                panel.origin.x() + PanelPadding + static_cast<float>(i) * IconAdvance,
                eggY
            },
            IconScale,
            Magnum::Color4{0.95f, 0.90f, 0.70f, 1.0f}
        );
    }

    numberRenderer.drawNumber(
        projection,
        Magnum::Vector2{
            panel.origin.x() + ResourceBarStartX + ResourceBarMaxWidth + 10.0f,
            eggY + 2.0f
        },
        summary.eggCount,
        0.75f,
        Magnum::Color4{0.95f, 0.90f, 0.70f, 1.0f}
    );

    (void)shader;
}
}

TileInfoPanelRenderer::TileInfoPanelRenderer(Magnum::Shaders::FlatGL2D &shader)
    : _shader(shader),
      _numberRenderer(shader),
      _iconRenderer(shader)

{
}

void TileInfoPanelRenderer::draw(
    const GameState &state,
    const std::optional<Magnum::Vector2i> &selectedTile,
    const Magnum::Vector2i &framebufferSize
)
{
    if (!selectedTile.has_value())
        return;

    if (framebufferSize.x() <= 0 || framebufferSize.y() <= 0)
        return;

    const TileSummary summary = summarizeTile(state, selectedTile.value());
    const PanelGeometry panel = panelGeometry(framebufferSize);
    const Magnum::Matrix3 projection = screenProjection(framebufferSize);

    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);

    drawQuad(
        _shader,
        projection,
        panel.origin,
        panel.size,
        Magnum::Color4{0.04f, 0.04f, 0.07f, 0.88f}
    );

    drawQuad(
        _shader,
        projection,
        panel.origin + Magnum::Vector2{0.0f, PanelHeight - HeaderHeight},
        Magnum::Vector2{PanelWidth, HeaderHeight},
        Magnum::Color4{0.12f, 0.12f, 0.18f, 0.95f}
    );

    drawResourceRows(
        _shader,
        _numberRenderer,
        projection,
        panel,
        summary
    );

    drawEntityRows(
        _shader,
        _numberRenderer,
        _iconRenderer,
        projection,
        panel,
        summary
    );

    drawCloseButton(
        _shader,
        projection,
        panel
    );

    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::FaceCulling);
}

bool TileInfoPanelRenderer::isCloseButtonAt(
    const Magnum::Vector2i &framebufferPosition,
    const Magnum::Vector2i &framebufferSize
) const
{
    if (framebufferSize.x() <= 0 || framebufferSize.y() <= 0)
        return false;

    const PanelGeometry panel = panelGeometry(framebufferSize);
    const Magnum::Vector2 buttonPosition = closeButtonPosition(panel);

    return containsPoint(
        buttonPosition,
        Magnum::Vector2{CloseButtonSize, CloseButtonSize},
        framebufferPosition
    );
}
}