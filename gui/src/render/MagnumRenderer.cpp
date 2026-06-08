#include "render/MagnumRenderer.hpp"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix3.h>

MagnumRenderer::MagnumRenderer(const Arguments &arguments)
    : Magnum::Platform::Sdl2Application(
          arguments,
          Configuration{}
              .setTitle("Zappy GUI")
              .setSize({1280, 720})
      ),
      _state(nullptr),
      _isOpen(true),
      _shader(),
      _camera(),
      _mapRenderer(_shader),
      _resourceRenderer(_shader),
      _incantationRenderer(_shader),
      _eggRenderer(_shader),
      _playerRenderer(_shader)
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
        const Magnum::Matrix3 projection =
            _camera.projection(_state->width(), _state->height(), framebufferSize());

        _mapRenderer.draw(*_state, projection);
        _resourceRenderer.draw(*_state, projection);
        _incantationRenderer.draw(*_state, projection);
        _eggRenderer.draw(*_state, projection);
        _playerRenderer.draw(*_state, projection);
    }

    swapBuffers();
}

void MagnumRenderer::viewportEvent(ViewportEvent &event)
{
    Magnum::GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
}