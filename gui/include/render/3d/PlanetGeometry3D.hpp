#pragma once

#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Vector3.h>

namespace zappy::render3d {

class PlanetGeometry3D {
public:
    static float radius(int mapWidth, int mapHeight);

    static Magnum::Vector3 tileCenter(
        float tileX,
        float tileY,
        int mapWidth,
        int mapHeight,
        float surfaceLift = 0.0f
    );

    static Magnum::Matrix4 surfaceTransform(
        float tileX,
        float tileY,
        int mapWidth,
        int mapHeight,
        float surfaceLift,
        float scale
    );

private:
    static float tileLongitude(float tileX, int mapWidth);
    static float tileLatitude(float tileY, int mapHeight);

    static Magnum::Vector3 spherePoint(
        float radius,
        float longitude,
        float latitude
    );

    static Magnum::Matrix4 alignUpToNormal(const Magnum::Vector3 &position);
};

}