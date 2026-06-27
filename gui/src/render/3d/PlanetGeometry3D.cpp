#include "render/3d/PlanetGeometry3D.hpp"

#include <Magnum/Math/Quaternion.h>

#include <algorithm>
#include <cmath>

namespace zappy::render3d {
namespace {

constexpr float Pi = 3.14159265358979323846f;
constexpr float PlanetRadiusScale = 0.45f;
constexpr float PlanetRadiusPadding = 2.0f;

}

float PlanetGeometry3D::radius(int mapWidth, int mapHeight)
{
    const float mapSize = static_cast<float>(std::max(mapWidth, mapHeight));

    return mapSize * PlanetRadiusScale + PlanetRadiusPadding;
}

Magnum::Vector3 PlanetGeometry3D::tileCenter(
    float tileX,
    float tileY,
    int mapWidth,
    int mapHeight,
    float surfaceLift
)
{
    return spherePoint(
        radius(mapWidth, mapHeight) + surfaceLift,
        tileLongitude(tileX, mapWidth),
        tileLatitude(tileY, mapHeight)
    );
}

Magnum::Matrix4 PlanetGeometry3D::surfaceTransform(
    float tileX,
    float tileY,
    int mapWidth,
    int mapHeight,
    float surfaceLift,
    float scale
)
{
    const Magnum::Vector3 position = tileCenter(
        tileX,
        tileY,
        mapWidth,
        mapHeight,
        surfaceLift
    );

    return alignUpToNormal(position) *
        Magnum::Matrix4::scaling(Magnum::Vector3{scale});
}

float PlanetGeometry3D::tileLongitude(float tileX, int mapWidth)
{
    return ((tileX + 0.5f) / static_cast<float>(mapWidth))
        * 2.0f * Pi;
}

float PlanetGeometry3D::tileLatitude(float tileY, int mapHeight)
{
    return -Pi * 0.5f
        + ((tileY + 0.5f) / static_cast<float>(mapHeight))
        * Pi;
}

Magnum::Vector3 PlanetGeometry3D::spherePoint(
    float radius,
    float longitude,
    float latitude
)
{
    const float cosLatitude = std::cos(latitude);

    return Magnum::Vector3{
        radius * cosLatitude * std::cos(longitude),
        radius * std::sin(latitude),
        radius * cosLatitude * std::sin(longitude)
    };
}

Magnum::Matrix4 PlanetGeometry3D::alignUpToNormal(
    const Magnum::Vector3 &position
)
{
    const Magnum::Vector3 normal = position.normalized();

    const Magnum::Quaternion alignment =
        Magnum::Quaternion::rotation(
            Magnum::Vector3::yAxis(),
            normal
        );

    return Magnum::Matrix4::from(
        alignment.toMatrix(),
        position
    );
}

}