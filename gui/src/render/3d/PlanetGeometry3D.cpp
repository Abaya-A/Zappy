#include "render/3d/PlanetGeometry3D.hpp"

#include <Magnum/Math/Quaternion.h>

#include <algorithm>
#include <cmath>

namespace zappy::render3d {
namespace {

constexpr float Pi = 3.14159265358979323846f;
constexpr float FullTurn = 2.0f * Pi;
constexpr float HalfTurn = 0.5f * Pi;

constexpr float TileCenterOffset = 0.5f;
constexpr float PlanetRadiusScale = 0.45f;
constexpr float PlanetRadiusPadding = 2.0f;

/*
** The 2D renderer displays X from left to right.
** The spherical projection visually wraps longitude in the opposite direction,
** so we mirror the normalized X coordinate here to keep 2D and 3D aligned.
*/
float normalizedMirroredTileCenter(float tileCoordinate, int mapSize)
{
    return 1.0f -
        ((tileCoordinate + TileCenterOffset) / static_cast<float>(mapSize));
}

float normalizedTileCenter(float tileCoordinate, int mapSize)
{
    return (tileCoordinate + TileCenterOffset) / static_cast<float>(mapSize);
}

float largestMapAxis(int mapWidth, int mapHeight)
{
    return static_cast<float>(std::max(mapWidth, mapHeight));
}

}

float PlanetGeometry3D::radius(int mapWidth, int mapHeight)
{
    return largestMapAxis(mapWidth, mapHeight) *
        PlanetRadiusScale + PlanetRadiusPadding;
}

Magnum::Vector3 PlanetGeometry3D::tileCenter(
    float tileX,
    float tileY,
    int mapWidth,
    int mapHeight,
    float surfaceLift
)
{
    const float planetRadius = radius(mapWidth, mapHeight) + surfaceLift;

    return spherePoint(
        planetRadius,
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
    return normalizedMirroredTileCenter(tileX, mapWidth) * FullTurn;
}

float PlanetGeometry3D::tileLatitude(float tileY, int mapHeight)
{
    return -HalfTurn + normalizedTileCenter(tileY, mapHeight) * Pi;
}

Magnum::Vector3 PlanetGeometry3D::spherePoint(
    float radius,
    float longitude,
    float latitude
)
{
    const float cosLatitude = std::cos(latitude);

    return {
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