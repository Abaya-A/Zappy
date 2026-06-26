#pragma once

#include "render/3d/RenderCamera3D.hpp"

#include <Magnum/Math/Vector2.h>

namespace zappy::render3d {

class PlanetCameraController {
public:
    bool rotateLeft(RenderCamera3D &camera);
    bool rotateRight(RenderCamera3D &camera);
    bool rotateUp(RenderCamera3D &camera);
    bool rotateDown(RenderCamera3D &camera);
    bool zoomIn(RenderCamera3D &camera);
    bool zoomOut(RenderCamera3D &camera);
    bool applyWheelZoom(RenderCamera3D &camera, float wheelY);
    bool reset(RenderCamera3D &camera);

    bool toggleMouseInvertX();
    bool toggleMouseInvertY();
    bool toggleMouseInvertBoth();

    bool startDrag();
    bool stopDrag();
    bool isDragging() const;

    bool applyMouseDrag(RenderCamera3D &camera, const Magnum::Vector2 &delta);

private:
    static constexpr float KeyRotationStep = 0.08f;
    static constexpr float MouseRotationSensitivity = 0.005f;
    static constexpr float KeyboardZoomStep = 0.35f;
    static constexpr float WheelZoomStep = 0.45f;

    bool _isDragging = false;
    bool _invertMouseX = false;
    bool _invertMouseY = false;
};

}