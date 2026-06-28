#include "render/3d/PlanetGeometry3D.hpp"

#include <Magnum/Math/Quaternion.h>

#include <algorithm>
#include <cmath>

namespace zappy::render3d {
namespace {

constexpr float Pi = 3.14159265358979323846f;
constexpr float FullTurn = 2.0f * Pi;

constexpr float TileCenterOffset = 0.5f;
constexpr float LatitudeLimit = Pi * 0.38f;

constexpr float PlanetRadiusScale = 0.45f;
constexpr float PlanetRadiusPadding = 2.0f;

float safeMapSize(int mapSize)
{
    return static_cast<float>(std::max(1, mapSize));
}

float largestMapAxis(int mapWidth, int mapHeight)
{
    return static_cast<float>(std::max(mapWidth, mapHeight));
}

float normalizedTileCenter(float tileCoordinate, int mapSize)
{
    return (tileCoordinate + TileCenterOffset) / safeMapSize(mapSize);
}

float mirroredNormalizedTileCenter(float tileCoordinate, int mapSize)
{
    return 1.0f - normalizedTileCenter(tileCoordinate, mapSize);
}

float planetRadiusFromMapSize(int mapWidth, int mapHeight)
{
    return largestMapAxis(mapWidth, mapHeight)
        * PlanetRadiusScale
        + PlanetRadiusPadding;
}

float latitudeFromNormalizedPosition(float normalizedY)
{
    return -LatitudeLimit + normalizedY * LatitudeLimit * 2.0f;
}

Magnum::Vector3 normalFromPosition(const Magnum::Vector3 &position)
{
    if (position.isZero())
        return Magnum::Vector3::yAxis();

    return position.normalized();
}

}

float PlanetGeometry3D::radius(int mapWidth, int mapHeight)
{
    return planetRadiusFromMapSize(mapWidth, mapHeight);
}

Magnum::Vector3 PlanetGeometry3D::tileCenter(
    float tileX,
    float tileY,
    int mapWidth,
    int mapHeight,
    float surfaceLift
)
{
    return surfacePoint(
        tileX,
        tileY,
        mapWidth,
        mapHeight,
        surfaceLift
    );
}

Magnum::Vector3 PlanetGeometry3D::surfacePoint(
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
    const Magnum::Vector3 position = surfacePoint(
        tileX,
        tileY,
        mapWidth,
        mapHeight,
        surfaceLift
    );

    return alignUpToNormal(position)
        * Magnum::Matrix4::scaling(Magnum::Vector3{scale});
}

float PlanetGeometry3D::tileLongitude(float tileX, int mapWidth)
{
    return mirroredNormalizedTileCenter(tileX, mapWidth) * FullTurn;
}

float PlanetGeometry3D::tileLatitude(float tileY, int mapHeight)
{
    return latitudeFromNormalizedPosition(
        normalizedTileCenter(tileY, mapHeight)
    );
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
    const Magnum::Vector3 normal = normalFromPosition(position);

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