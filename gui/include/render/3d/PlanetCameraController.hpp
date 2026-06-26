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
    bool reset(RenderCamera3D &camera);

    bool startDrag();
    bool stopDrag();
    bool isDragging() const;

    bool applyMouseDrag(RenderCamera3D &camera, const Magnum::Vector2 &delta);

private:
    static constexpr float KeyRotationStep = 0.08f;
    static constexpr float MouseRotationSensitivity = 0.005f;

    bool _isDragging = false;
};

}