#pragma once
#include "vec2.hpp"

struct Transform
{
    glm::vec2 position;
    glm::vec2 pivot_point;
    float rotation;

    Transform(const glm::vec2 position) : position(position), pivot_point(0.0f,0.0f), rotation(0.0f) {}
    Transform(const glm::vec2 position,const float rotation) : position(position), pivot_point(0.0f,0.0f), rotation(rotation) {}
};

struct VectorTransform : Transform
{
    glm::vec2 velocity;
    glm::vec2 acceleration;

    VectorTransform(const glm::vec2 position) : Transform(position), velocity(0,0), acceleration(0,0) {}
    VectorTransform(const glm::vec2 position,const float rotation) : Transform(position,rotation), velocity(0,0), acceleration(0,0) {};
};