#pragma once

#include <glm.hpp>

namespace world
{
    // chunk.h
    constexpr int CHUNK_SIZE = 8;
    // region.h
    constexpr int REGION_SIZE = 32;
    constexpr int STACK_SIZE = 4;
    // world.h
    constexpr float WORLD_SIZE = 10.0f; // since we want a block position to represent 1. 1/10 = 0.1
    constexpr int WORLD_WIDTH = 5; // min is 3x3
    constexpr int WORLD_HEIGHT = 3;

    extern glm::vec2 origin;
    extern glm::ivec2 chunk_origin;

    glm::ivec2 ToChunkSpace(glm::vec2 pos);
    bool ChunkToArray(glm::ivec2& chunk,glm::ivec2 origin = chunk_origin);
    void ArrayToChunk(glm::ivec2& chunk,glm::ivec2 origin = chunk_origin);
    void GlobalToChunk(glm::ivec2& position, glm::ivec2& chunk);
    void ChunkToGlobal(glm::ivec2& position,glm::ivec2 chunk);
}

namespace gl
{
    // light.h
    constexpr int LIGHT_MAP_SIZE = world::REGION_SIZE * world::CHUNK_SIZE;
}