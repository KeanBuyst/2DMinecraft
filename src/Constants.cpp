#include "Constants.h"

#include <iostream>

glm::vec2 world::origin = glm::vec2(CHUNK_SIZE / 2.0f,CHUNK_SIZE / 2.0f);
glm::ivec2 world::chunk_origin = ToChunkSpace(world::origin);

float world::delta_time = 0.0f;

bool world::OutOfBounds(glm::vec2 pos)
{
    glm::ivec2 arrayPos = ToChunkSpace(pos) - chunk_origin;
    arrayPos.x += (WORLD_WIDTH - 1) / 2;
    arrayPos.y += (WORLD_HEIGHT - 1) / 2;
    return (arrayPos.x < POST_GEN_BUF || arrayPos.x >= WORLD_WIDTH - POST_GEN_BUF)
        || (arrayPos.y < POST_GEN_BUF || arrayPos.y >= WORLD_HEIGHT - POST_GEN_BUF);
}

glm::ivec2 world::ToChunkSpace(const glm::vec2 pos)
{
    // fix division problem. E.g. -1 / 32 = 0 && 1 / 32 = 0
    const glm::ivec2 fix = glm::floor(pos / static_cast<float>(CHUNK_SIZE));
    return fix;
}

// Bool determins if its a successful choord in the chunk or its not within the loaded chunks
bool world::ChunkToArray(glm::ivec2& chunk, const glm::ivec2 origin)
{
    chunk -= origin;
    chunk.x += (WORLD_WIDTH - 1) / 2;
    chunk.y += (WORLD_HEIGHT - 1) / 2;
    return chunk.x >= 0 && chunk.x < WORLD_WIDTH && chunk.y >= 0 && chunk.y < WORLD_HEIGHT;
}

void world::ArrayToChunk(glm::ivec2& chunk, const glm::ivec2 origin)
{
    chunk += origin;
    chunk.x -= (WORLD_WIDTH - 1) / 2;
    chunk.y -= (WORLD_HEIGHT - 1) / 2;
}
// Position to chunk local position and chunk position
void world::GlobalToChunk(glm::ivec2& position,glm::ivec2& chunk)
{
    chunk = ToChunkSpace(position);
    if (const int x = position.x; x < 0)
        position.x = (CHUNK_SIZE - 1) + (x + 1) % CHUNK_SIZE;
    else position.x = x % CHUNK_SIZE;
    if (const int y = position.y; y < 0)
        position.y = (CHUNK_SIZE - 1) + (y + 1) % CHUNK_SIZE;
    else position.y = y % CHUNK_SIZE;
}
// Position in chunk to global position
void world::ChunkToGlobal(glm::ivec2& position,glm::ivec2 chunk)
{
    if (chunk.x < 0)
    {
        chunk.x++;
        position.x -= CHUNK_SIZE;
    }
    if (chunk.y < 0)
    {
        chunk.y++;
        position.y -= CHUNK_SIZE;
    }
    position += chunk * CHUNK_SIZE;
}