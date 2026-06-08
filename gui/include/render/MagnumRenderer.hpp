#pragma once

#include "state/GameState.hpp"

#include <Magnum/GL/Mesh.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Shaders/FlatGL.h>
#include <Magnum/Math/Matrix3.h>

class MagnumRenderer : public Magnum::Platform::Sdl2Application {
public:
    explicit MagnumRenderer(const Arguments &arguments);

    bool isOpen() const;
    void handleEvents();
    void render(const GameState &state);

private:
    void drawEvent() override;
    void drawMapGrid(const GameState &state);
    void drawTileResources(const GameState &state);
    void drawPlayers(const GameState &state);
    void drawEggs(const GameState &state);
    void viewportEvent(ViewportEvent &event) override;
    Magnum::Matrix3 buildCameraProjection(int width, int height) const;

    const GameState *_state;
    bool _isOpen;
    Magnum::Shaders::FlatGL2D _shader;
};