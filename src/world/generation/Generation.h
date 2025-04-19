#pragma once

#include "../World.h"

namespace world::Generate
{
    void Lighting(Chunk& chunk);

    bool OreGeneration(glm::vec2 position,int depth, MATERIAL &ore);
}