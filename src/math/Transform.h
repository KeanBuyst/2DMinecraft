#pragma once

#include "vec2.hpp"
#include <iostream>

#include "../Util.h"

struct Transform
{
    glm::vec2 position;
    glm::vec2 pivot_point;
    float rotation;

    Transform(Util::ByteStream& stream) : pivot_point(0.0f,0.0f)
    {
        stream >> position.x;
        stream >> position.y;
        stream >> rotation;
    }
    Transform(const glm::vec2 position) : position(position), pivot_point(0.0f,0.0f), rotation(0.0f) {}
    Transform(const glm::vec2 position,const float rotation) : position(position), pivot_point(0.0f,0.0f), rotation(rotation) {}

    void serialize(Util::ByteStream& stream)
    {
        stream << position.x;
        stream << position.y;
        stream << rotation;
    }
};