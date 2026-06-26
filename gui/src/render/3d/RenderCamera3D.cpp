#include "render/3d/RenderCamera3D.hpp"

#include <Magnum/Math/Angle.h>
#include <Magnum/Math/Matrix4.h>

#include <algorithm>
#include <cmath>

namespace zappy::render3d {
namespace {

constexpr float PlanetRadiusScale = 0.45f;
constexpr float PlanetRadiusPadding = 2.0f;
constexpr float DefaultDistanceScale = 5.2f;
constexpr float MinDistanceScale = 2.4f;
constexpr float MaxDistanceScale = 10.0f;
constexpr float FieldOfView = 35.0f;
constexpr float NearPlane = 0.1f;
constexpr float FarPlaneMultiplier = 12.0f;
constexpr float DefaultYaw = 0.0f;
constexpr float DefaultPitch = 0.25f;
constexpr float MinPitch = -1.15f;
constexpr float MaxPitch = 1.15f;

float computePlanetRadius(int mapWidth, int mapHeight)
{
    const float width = static_cast<float>(mapWidth > 0 ? mapWidth : 1);
    const float height = static_cast<float>(mapHeight > 0 ? mapHeight : 1);
    const float mapSize = std::max(width, height);

    return mapSize * PlanetRadiusScale + PlanetRadiusPadding;
}

float computeAspectRatio(const Magnum::Vector2i &framebufferSize)
{
    if (framebufferSize.y() == 0)
        return 1.0f;

    return static_cast<float>(framebufferSize.x()) /
        static_cast<float>(framebufferSize.y());
}

}

RenderCamera3D::RenderCamera3D()
    : _yaw(DefaultYaw),
      _pitch(DefaultPitch),
      _distanceScale(DefaultDistanceScale)
{
}

Magnum::Matrix4 RenderCamera3D::projection(
    int mapWidth,
    int mapHeight,
    const Magnum::Vector2i &framebufferSize
) const
{
    const float radius = computePlanetRadius(mapWidth, mapHeight);
    const float distance = radius * _distanceScale;
    const float aspect = computeAspectRatio(framebufferSize);
    const float cosPitch = std::cos(_pitch);

    const Magnum::Vector3 center{0.0f, 0.0f, 0.0f};

    const Magnum::Vector3 eye{
        distance * std::sin(_yaw) * cosPitch,
        distance * std::sin(_pitch),
        distance * std::cos(_yaw) * cosPitch
    };

    const Magnum::Matrix4 projection =
        Magnum::Matrix4::perspectiveProjection(
            Magnum::Deg{FieldOfView},
            aspect,
            NearPlane,
            radius * FarPlaneMultiplier
        );

    const Magnum::Matrix4 camera =
        Magnum::Matrix4::lookAt(
            eye,
            center,
            Magnum::Vector3::yAxis()
        );

    return projection * camera.inverted();
}

void RenderCamera3D::rotate(float yawDelta, float pitchDelta)
{
    _yaw += yawDelta;
    _pitch = std::clamp(_pitch + pitchDelta, MinPitch, MaxPitch);
}

void RenderCamera3D::zoom(float delta)
{
    _distanceScale = std::clamp(
        _distanceScale + delta,
        MinDistanceScale,
        MaxDistanceScale
    );
}

void RenderCamera3D::reset()
{
    _yaw = DefaultYaw;
    _pitch = DefaultPitch;
    _distanceScale = DefaultDistanceScale;
}

}