#include "render/MagnumRenderer.hpp"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>

MagnumRenderer::MagnumRenderer(const Arguments &arguments)
    : Magnum::Platform::Sdl2Application(
          arguments,
          Configuration{}
              .setTitle("Zappy GUI")
              .setSize({1280, 720})
      ),
      _state(nullptr),
      _isOpen(true)
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
    swapBuffers();
}