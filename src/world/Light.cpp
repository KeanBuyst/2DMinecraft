#include "World.h"

#include <iostream>

// these number get interpreted as negative except for 0 which is interpreted as daylight
constexpr int SOLID = -3;
constexpr int TRANSPARENT = -1;

int world::DAYLIGHT = 15; // TODO make this one change with the time of day

void world::updateLighting(const Chunk* surroundings, Chunk& chunk)
{
    // sweep all tiles to with default light values and TOP to DOWN attenuation
    for (auto x = 0; x < CHUNK_SIZE; ++x)
    {
        // reset light column
        chunk.lightMap[x] = 0;
        int last = DAYLIGHT;
        // compute top light
        if (surroundings[0].blocks[x] != 0)
        {
            const Block bottom = surroundings[0].getBlock(glm::ivec2(x,CHUNK_SIZE - 1));
            if (bottom.luminance == 0)
            {
                for (auto y = 2; y <= CHUNK_SIZE; ++y)
                {
                    Block block = surroundings[0].getBlock(glm::ivec2(x,CHUNK_SIZE - y));
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
            } else if (bottom.isEmpty())
                last = bottom.luminance;
            else if (bottom.isTransparent())
                last = bottom.luminance + TRANSPARENT;
            else
                last = bottom.luminance + SOLID;
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