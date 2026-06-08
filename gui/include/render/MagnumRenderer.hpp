#pragma once

#include "render/EggRenderer.hpp"
#include "render/MapRenderer.hpp"
#include "render/PlayerRenderer.hpp"
#include "render/RenderCamera.hpp"
#include "render/ResourceRenderer.hpp"
#include "state/GameState.hpp"
#include "render/IncantationRenderer.hpp"

#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Shaders/FlatGL.h>

class MagnumRenderer : public Magnum::Platform::Sdl2Application {
public:
    explicit MagnumRenderer(const Arguments &arguments);

    bool isOpen() const;
    void handleEvents();
    void render(const GameState &state);

private:
    void drawEvent() override;
    void viewportEvent(ViewportEvent &event) override;

    const GameState *_state;
    bool _isOpen;

    Magnum::Shaders::FlatGL2D _shader;

    zappy::render::RenderCamera _camera;
    zappy::render::MapRenderer _mapRenderer;
    zappy::render::ResourceRenderer _resourceRenderer;
    zappy::render::IncantationRenderer _incantationRenderer;
    zappy::render::EggRenderer _eggRenderer;
    zappy::render::PlayerRenderer _playerRenderer;
};