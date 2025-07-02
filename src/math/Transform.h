#pragma once
#include "vec2.hpp"

struct Transform
{
    Transform(const glm::vec2 position) : position(position), rotation(0.0f) {}
    Transform(const glm::vec2 position,const float rotation) : position(position), rotation(rotation) {}

    glm::vec2 position;
    float rotation;
};
