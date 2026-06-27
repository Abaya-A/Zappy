#include "render/MagnumRenderer.hpp"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Matrix4.h>

#include <algorithm>

namespace {

constexpr Magnum::Color4 ClearColor{0.06f, 0.06f, 0.09f, 1.0f};

constexpr int MinimapWidth = 300;
constexpr int MinimapHeight = 260;
constexpr int MinimapMargin = 25;
constexpr int MinimapInnerPadding = 12;

}

MagnumRenderer::MagnumRenderer(const Arguments &arguments)
    : Magnum::Platform::Sdl2Application(
          arguments,
          Configuration{}
              .setTitle("Zappy GUI")
              .setSize({1280, 720})
      ),
      _state(nullptr),
      _isOpen(true),
      _showMinimap(false),
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
      _planetCameraController(),
      _mapRenderer3D(_shader3D),
      _resourceModelRenderer3D(_shader3D),
      _eggModelRenderer3D(_shader3D),
      _playerModelRenderer3D(_shader3D)
{
    configureRenderer();
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

void MagnumRenderer::configureRenderer()
{
    Magnum::GL::Renderer::setClearColor(ClearColor);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::FaceCulling);
}

void MagnumRenderer::clearFrame()
{
    Magnum::GL::defaultFramebuffer.clear(
        Magnum::GL::FramebufferClear::Color |
        Magnum::GL::FramebufferClear::Depth
    );
}

bool MagnumRenderer::canRender() const
{
    return _state != nullptr && _state->isReady();
}

void MagnumRenderer::drawEvent()
{
    clearFrame();

    if (canRender())
        drawScene();

    swapBuffers();
}

void MagnumRenderer::drawScene()
{
    drawMain3DView();

    if (_showMinimap)
        drawMinimapViewport();
}

void MagnumRenderer::drawMain3DView()
{
    restoreFullViewport();

    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::FaceCulling);

    const Magnum::Matrix4 projection =
        _camera3D.projection(_state->width(), _state->height(), framebufferSize());

    _mapRenderer3D.draw(*_state, projection);
    _resourceModelRenderer3D.draw(*_state, projection);
    _eggModelRenderer3D.draw(*_state, projection);
    _playerModelRenderer3D.draw(*_state, projection);
}

void MagnumRenderer::drawMinimapViewport()
{
    const Magnum::Vector2i fullSize = framebufferSize();

    if (fullSize.x() <= 0 || fullSize.y() <= 0)
        return;

    const Magnum::Vector2i minimapSize{
        std::min(MinimapWidth, fullSize.x()),
        std::min(MinimapHeight, fullSize.y())
    };

    const Magnum::Vector2i minimapPosition{
        MinimapMargin,
        MinimapMargin
    };

    const Magnum::Vector2i innerPosition{
        minimapPosition.x() + MinimapInnerPadding,
        minimapPosition.y() + MinimapInnerPadding
    };

    const Magnum::Vector2i innerSize{
        std::max(1, minimapSize.x() - MinimapInnerPadding * 2),
        std::max(1, minimapSize.y() - MinimapInnerPadding * 2)
    };

    Magnum::GL::defaultFramebuffer.setViewport({
        innerPosition,
        innerSize
    });

    drawMinimapContent(innerSize);
    restoreFullViewport();
}

void MagnumRenderer::drawMinimapContent(const Magnum::Vector2i &viewportSize)
{
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);

    const Magnum::Matrix3 projection =
        _camera.projection(_state->width(), _state->height(), viewportSize);

    _mapRenderer.draw(*_state, projection);
    _resourceRenderer.draw(*_state, projection);
    _eggRenderer.draw(*_state, projection);
    _playerRenderer.draw(*_state, projection);
    _incantationRenderer.draw(*_state, projection);
    _broadcastRenderer.draw(*_state, projection);
    _expulsionRenderer.draw(*_state, projection);

    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::FaceCulling);
}

void MagnumRenderer::restoreFullViewport()
{
    Magnum::GL::defaultFramebuffer.setViewport({{}, framebufferSize()});
}

void MagnumRenderer::viewportEvent(ViewportEvent &event)
{
    Magnum::GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
}

bool MagnumRenderer::handleKeyRotation(KeyEvent &event)
{
    if (event.key() == Key::Left)
        return _planetCameraController.rotateLeft(_camera3D);

    if (event.key() == Key::Right)
        return _planetCameraController.rotateRight(_camera3D);

    if (event.key() == Key::Up)
        return _planetCameraController.rotateUp(_camera3D);

    if (event.key() == Key::Down)
        return _planetCameraController.rotateDown(_camera3D);

    if (event.key() == Key::R)
        return _planetCameraController.reset(_camera3D);

    return false;
}

bool MagnumRenderer::handleZoomKey(KeyEvent &event)
{
    if (event.key() == Key::Plus || event.key() == Key::NumAdd)
        return _planetCameraController.zoomIn(_camera3D);

    if (event.key() == Key::Minus || event.key() == Key::NumSubtract)
        return _planetCameraController.zoomOut(_camera3D);

    return false;
}

bool MagnumRenderer::handleMouseSettingsKey(KeyEvent &event)
{
    if (event.key() == Key::X)
        return _planetCameraController.toggleMouseInvertX();

    if (event.key() == Key::Y)
        return _planetCameraController.toggleMouseInvertY();

    if (event.key() == Key::I)
        return _planetCameraController.toggleMouseInvertBoth();

    return false;
}

bool MagnumRenderer::handleMinimapKey(KeyEvent &event)
{
    if (event.key() != Key::M)
        return false;

    _showMinimap = !_showMinimap;
    return true;
}

void MagnumRenderer::keyPressEvent(KeyEvent &event)
{
    if (!handleKeyRotation(event) &&
        !handleZoomKey(event) &&
        !handleMouseSettingsKey(event) &&
        !handleMinimapKey(event))
        return;

    event.setAccepted();
    redrawAfterInput();
}

void MagnumRenderer::scrollEvent(ScrollEvent &event)
{
    if (!_planetCameraController.applyWheelZoom(_camera3D, event.offset().y()))
        return;

    event.setAccepted();
    redrawAfterInput();
}

void MagnumRenderer::pointerPressEvent(PointerEvent &event)
{
    if (!event.isPrimary())
        return;

    _planetCameraController.startDrag();
    event.setAccepted();
}

void MagnumRenderer::pointerReleaseEvent(PointerEvent &event)
{
    if (!event.isPrimary())
        return;

    _planetCameraController.stopDrag();
    event.setAccepted();
}

void MagnumRenderer::pointerMoveEvent(PointerMoveEvent &event)
{
    if (!_planetCameraController.applyMouseDrag(
            _camera3D,
            event.relativePosition()
        ))
        return;

    event.setAccepted();
    redrawAfterInput();
}

void MagnumRenderer::redrawAfterInput()
{
    redraw();
}