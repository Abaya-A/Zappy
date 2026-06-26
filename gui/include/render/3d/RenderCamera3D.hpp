#pragma once

#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Vector2.h>

namespace zappy::render3d {

class RenderCamera3D {
public:
    RenderCamera3D();

    Magnum::Matrix4 projection(
        int mapWidth,
        int mapHeight,
        const Magnum::Vector2i &framebufferSize
    ) const;

    void rotate(float yawDelta, float pitchDelta);
    void zoom(float delta);
    void reset();

private:
    float _yaw;
    float _pitch;
    float _distanceScale;
};

}