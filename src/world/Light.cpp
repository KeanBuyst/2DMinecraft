#include "World.h"

#include <iostream>

// these number get interpreted as negative except for 0 which is interpreted as daylight
constexpr int SOLID = -3;
constexpr int TRANSPARENT = -1;

int world::DAYLIGHT = 15; // TODO make this one change with the time of day

void world::updateLighting(const Chunk* surroundings, Chunk& chunk)
{
    // inversed daylight

    // sweep all tiles to with default light values and TOP to DOWN attenuation
    for (auto x = 0; x < CHUNK_SIZE; ++x)
    {
        // reset light column
        chunk.lightMap[x] = 0;
        int last = DAYLIGHT;
        // compute top light
        if (surroundings[1].blocks[x] != 0)
        {
            for (auto y = 1; y <= CHUNK_SIZE; ++y)
            {
                Block block = surroundings[6].getBlock(glm::ivec2(x,CHUNK_SIZE - y));
                if (block.isEmpty())
                    break;
                if (block.isTransparent())
                    last += TRANSPARENT;
                else last += SOLID;
                if (last <= 0)
                {
                    last = 0;
                    break;
                }
            }
        }
        // TOP to DOWN attenuation
        for (auto y = 0; y < CHUNK_SIZE; ++y)
        {
            if (last < 0)
                 break;
            const glm::ivec2 pos(x,y);
            Block block = chunk.getBlock(pos);
            if (block.isEmpty())
            {
                block.luminance = DAYLIGHT;
                last = DAYLIGHT;
            }
            else if (block.isTransparent())
            {
                block.luminance = last;
                last += TRANSPARENT;
            }
            else
            {
                block.luminance = last;
                last += SOLID;
            }
            chunk.setBlock(pos,block);
        }
    }
}