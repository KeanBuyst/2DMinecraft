#pragma once

#include <glm/glm.hpp>

namespace world
{
    // chunk.h
    constexpr int CHUNK_SIZE = 16;
    // region.h
    constexpr int REGION_SIZE = 32;
    constexpr int STACK_SIZE = 4;
    // world.h
    constexpr float WORLD_SIZE = 10.0f; // since we want a block position to represent 1. 1/10 = 0.1
    constexpr int WORLD_WIDTH = 7; // min is 3x3
    constexpr int WORLD_HEIGHT = 5;
    constexpr int POST_GEN_BUF = 1; // must be twice as less as WORLD_(WIDTH and HEIGHT)
    // physics
    constexpr float GRAVITY = 48.0f;
    constexpr float DRAG = 2.4f;
    constexpr float FRICTION = 14.6f;
    constexpr float MAX_SPEED = 30.0f;
    // entities
    constexpr uint32_t MAX_SPRITES_INIT = 1024; // 2^10

    extern glm::vec2 origin;
    extern glm::ivec2 chunk_origin;

    extern float delta_time;

    bool OutOfBounds(glm::vec2 pos);
    glm::ivec2 ToChunkSpace(glm::vec2 pos);
    bool ChunkToArray(glm::ivec2& chunk,glm::ivec2 origin = chunk_origin);
    void ArrayToChunk(glm::ivec2& chunk,glm::ivec2 origin = chunk_origin);
    void GlobalToChunk(glm::ivec2& position, glm::ivec2& chunk);
    void ChunkToGlobal(glm::ivec2& position,glm::ivec2 chunk);
}

constexpr int TILES_PER_ROW = 16;
constexpr float BLOCK_SIZE = 16.0f;
constexpr float TILE_UV_SIZE = 1.0f / BLOCK_SIZE;
constexpr float GUI_SCALE = 4.0f;
constexpr static glm::vec2 SLOT_SIZE = glm::vec2(12.0f * GUI_SCALE,12.0f * GUI_SCALE);
// view port is from -VIEW_SIZE to VIEW_SIZE,
// horizontal is also effected by aspect ratio
constexpr float VIEW_SCALE = world::CHUNK_SIZE * BLOCK_SIZE;
extern glm::vec2 VIEW_SIZE;