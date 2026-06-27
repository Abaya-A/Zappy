#pragma once

#include "state/GameState.hpp"
#include "render/HudNumberRenderer.hpp"
#include "render/HudIconRenderer.hpp"

#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Shaders/FlatGL.h>

#include <optional>

namespace zappy::render {

class TileInfoPanelRenderer {
public:
    explicit TileInfoPanelRenderer(Magnum::Shaders::FlatGL2D &shader);

    void draw(const GameState &state, const std::optional<Magnum::Vector2i> &selectedTile, const Magnum::Vector2i &framebufferSize);

    bool isCloseButtonAt(const Magnum::Vector2i &framebufferPosition, const Magnum::Vector2i &framebufferSize ) const;

private:
    Magnum::Shaders::FlatGL2D &_shader;
    HudNumberRenderer _numberRenderer;    
    HudIconRenderer _iconRenderer;
};

}