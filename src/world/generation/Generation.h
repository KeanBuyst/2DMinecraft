#pragma once

#include "../World.h"

namespace world::Generate
{
    void Lighting(World* world,glm::ivec2 chunk_pos);

    void BlockBorder(World* world, glm::ivec2 chunk_pos);

    bool OreGeneration(glm::vec2 position,int depth, BlockType &ore);

    bool CaveGeneration(glm::vec2 position,int depth);
}