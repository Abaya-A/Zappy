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
      _broadcastRenderer(_shader),
      _expulsionRenderer(_shader),
      _eggRenderer(_shader),
      _playerRenderer(_shader),
      _shader3D(),
      _camera3D(),
      _mapRenderer3D(_shader3D)
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

        const Magnum::Matrix4 projection3D =
            _camera3D.projection(_state->width(), _state->height(), framebufferSize());

        _mapRenderer3D.draw(*_state, projection3D);
    }

    swapBuffers();
}

void MagnumRenderer::viewportEvent(ViewportEvent &event)
{
    Magnum::GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
}