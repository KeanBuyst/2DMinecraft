#pragma once

#include "../World.h"

namespace world::Generate
{
    void Lighting(World* world,glm::ivec2 chunk_pos);

    bool OreGeneration(glm::vec2 position,int depth, MATERIAL &ore);
}