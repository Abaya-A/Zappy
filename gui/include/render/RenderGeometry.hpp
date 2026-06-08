#pragma once

#include "state/GameState.hpp"

#include <Magnum/Math/Color.h>
#include <Magnum/Math/Vector2.h>

#include <array>
#include <vector>

namespace zappy::render {

struct Vertex {
    Magnum::Vector2 position;
};

constexpr int ResourceCount = 7;

struct ResourceMarkerStyle {
    Magnum::Color4 color;
    float offsetX;
    float offsetY;
};

extern const std::array<ResourceMarkerStyle, ResourceCount> ResourceMarkerStyles;

std::vector<Vertex> buildGridVertices(int width, int height);
std::array<std::vector<Vertex>, ResourceCount> buildResourceVertices(const GameState &state);
std::vector<Vertex> buildEggVertices(const GameState &state);
std::vector<Vertex> buildPlayerVertices(const GameState &state);

}