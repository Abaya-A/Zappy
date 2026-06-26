#include "render/3d/PlanetCameraController.hpp"

namespace zappy::render3d {

bool PlanetCameraController::rotateLeft(RenderCamera3D &camera)
{
    camera.rotate(-KeyRotationStep, 0.0f);
    return true;
}

bool PlanetCameraController::rotateRight(RenderCamera3D &camera)
{
    camera.rotate(KeyRotationStep, 0.0f);
    return true;
}

bool PlanetCameraController::rotateUp(RenderCamera3D &camera)
{
    camera.rotate(0.0f, KeyRotationStep);
    return true;
}

bool PlanetCameraController::rotateDown(RenderCamera3D &camera)
{
    camera.rotate(0.0f, -KeyRotationStep);
    return true;
}

bool PlanetCameraController::zoomIn(RenderCamera3D &camera)
{
    camera.zoom(-KeyboardZoomStep);
    return true;
}

bool PlanetCameraController::zoomOut(RenderCamera3D &camera)
{
    camera.zoom(KeyboardZoomStep);
    return true;
}

bool PlanetCameraController::applyWheelZoom(RenderCamera3D &camera, float wheelY)
{
    if (wheelY == 0.0f)
        return false;

    camera.zoom(-wheelY * WheelZoomStep);
    return true;
}

bool PlanetCameraController::reset(RenderCamera3D &camera)
{
    camera.reset();
    return true;
}

bool PlanetCameraController::toggleMouseInvertX()
{
    _invertMouseX = !_invertMouseX;
    return true;
}

bool PlanetCameraController::toggleMouseInvertY()
{
    _invertMouseY = !_invertMouseY;
    return true;
}

bool PlanetCameraController::toggleMouseInvertBoth()
{
    _invertMouseX = !_invertMouseX;
    _invertMouseY = !_invertMouseY;
    return true;
}

bool PlanetCameraController::startDrag()
{
    _isDragging = true;
    return true;
}

bool PlanetCameraController::stopDrag()
{
    _isDragging = false;
    return true;
}

bool PlanetCameraController::isDragging() const
{
    return _isDragging;
}

bool PlanetCameraController::applyMouseDrag(
    RenderCamera3D &camera,
    const Magnum::Vector2 &delta
)
{
    if (!_isDragging)
        return false;

    const float xDirection = _invertMouseX ? -1.0f : 1.0f;
    const float yDirection = _invertMouseY ? -1.0f : 1.0f;

    camera.rotate(
        delta.x() * MouseRotationSensitivity * xDirection,
        delta.y() * MouseRotationSensitivity * yDirection
    );

    return true;
}

}