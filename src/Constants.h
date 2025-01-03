#pragma once

namespace world
{
    // chunk.h
    constexpr int CHUNK_SIZE = 8;
    // region.h
    constexpr int REGION_SIZE = 32;
    constexpr int STACK_SIZE = 4;
    // world.h
    constexpr float WORLD_SIZE = 10.0f; // since we want a block postion to represent 1. 1/10 = 0.1
    constexpr int WORLD_WIDTH = 5; // min is 3x3
    constexpr int WORLD_HEIGHT = 5;
}

namespace gl
{
    // light.h
    constexpr int LIGHT_MAP_SIZE = world::REGION_SIZE * world::CHUNK_SIZE;
}