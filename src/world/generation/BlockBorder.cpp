#include "Generation.h"

inline bool shouldOutLine(const world::Block& block)
{
    return !block.isCollidable() && (block.getType() != world::EMPTY || !block.isTransparent()) && block.getWall() != world::EMPTY;
}

enum SIDES : uint8_t
{
    TOP = 0b0001,
    RIGHT = 0b0010,
    BOTTOM = 0b0100,
    LEFT = 0b1000
};

void world::Generate::BlockBorder(World* world, glm::ivec2 chunk_pos)
{
    Chunk chunk = world->getChunk(chunk_pos);
    glm::ivec2 globalChunkPos(0,0);
    ChunkToGlobal(globalChunkPos,chunk_pos);

     for (auto y = 0; y < CHUNK_SIZE; ++y)
     {
         for (auto x = 0; x < CHUNK_SIZE; ++x)
         {
             const glm::ivec2 local_pos(x,y);
             const glm::vec2 pos = globalChunkPos + local_pos;
             Block block = chunk.getBlock(local_pos);
             uint8_t sides = 0;
             if (!block.isTransparent() && block.isCollidable())
             {
                 const glm::vec2 left(pos.x - 1.0f,pos.y);
                 const glm::vec2 right(pos.x + 1.0f,pos.y);
                 const glm::vec2 top(pos.x,pos.y + 1.0f);
                 const glm::vec2 bottom(pos.x,pos.y - 1.0f);

                 Block b_top = world->getBlock(top);
                 if (shouldOutLine(b_top))
                 {
                     sides |= TOP;
                 }
                 Block b_right = world->getBlock(right);
                 if (shouldOutLine(b_right))
                 {
                     sides |= RIGHT;
                 }
                 Block b_bottom = world->getBlock(bottom);
                 if (shouldOutLine(b_bottom))
                 {
                     sides |= BOTTOM;
                 }
                 Block b_left = world->getBlock(left);
                 if (shouldOutLine(b_left))
                 {
                     sides |= LEFT;
                 }
             }
             chunk.setBlockBorder(local_pos,sides);
         }
     }
    world->setChunk(chunk);
}