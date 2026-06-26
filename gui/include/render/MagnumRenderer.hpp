#pragma once

#include "render/3d/MapRenderer3D.hpp"
#include "render/3d/PlanetCameraController.hpp"
#include "render/3d/RenderCamera3D.hpp"
#include "render/BroadcastRenderer.hpp"
#include "render/EggRenderer.hpp"
#include "render/ExpulsionRenderer.hpp"
#include "render/IncantationRenderer.hpp"
#include "render/MapRenderer.hpp"
#include "render/PlayerRenderer.hpp"
#include "render/RenderCamera.hpp"
#include "render/ResourceRenderer.hpp"
#include "state/GameState.hpp"
#include "render/3d/ResourceModelRenderer3D.hpp"

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
    void keyPressEvent(KeyEvent &event) override;
    void pointerPressEvent(PointerEvent &event) override;
    void pointerReleaseEvent(PointerEvent &event) override;
    void pointerMoveEvent(PointerMoveEvent &event) override;
    void scrollEvent(ScrollEvent &event) override;

    void configureRenderer();
    void clearFrame();
    bool canRender() const;

    void draw3DMap();
    bool handleKeyRotation(KeyEvent &event);
    bool handleZoomKey(KeyEvent &event);
    void redrawAfterInput();
    bool handleMouseSettingsKey(KeyEvent &event);
    
    const GameState *_state;
    bool _isOpen;

    Magnum::Shaders::FlatGL2D _shader;
    zappy::render::RenderCamera _camera;
    zappy::render::MapRenderer _mapRenderer;
    zappy::render::ResourceRenderer _resourceRenderer;
    zappy::render::IncantationRenderer _incantationRenderer;
    zappy::render::BroadcastRenderer _broadcastRenderer;
    zappy::render::ExpulsionRenderer _expulsionRenderer;
    zappy::render::EggRenderer _eggRenderer;
    zappy::render::PlayerRenderer _playerRenderer;

    Magnum::Shaders::FlatGL3D _shader3D;
    zappy::render3d::RenderCamera3D _camera3D;
    zappy::render3d::PlanetCameraController _planetCameraController;
    zappy::render3d::MapRenderer3D _mapRenderer3D;
    zappy::render3d::ResourceModelRenderer3D _resourceModelRenderer3D;
};