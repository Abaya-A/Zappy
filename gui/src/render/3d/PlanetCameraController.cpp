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

bool PlanetCameraController::reset(RenderCamera3D &camera)
{
    camera.reset();
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

    camera.rotate(
        -delta.x() * MouseRotationSensitivity,
        -delta.y() * MouseRotationSensitivity
    );

    return true;
}
}