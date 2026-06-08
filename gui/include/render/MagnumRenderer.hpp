#pragma once

#include "state/GameState.hpp"

#include <Magnum/Platform/Sdl2Application.h>

class MagnumRenderer : public Magnum::Platform::Sdl2Application {
public:
    explicit MagnumRenderer(const Arguments &arguments);

    bool isOpen() const;
    void handleEvents();
    void render(const GameState &state);

private:
    void drawEvent() override;

    const GameState *_state;
    bool _isOpen;
};