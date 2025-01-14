#include "World.h"

#include <iostream>

// these number get interpreted as negative except for 0 which is interpreted as daylight
constexpr int SOLID = -3;
constexpr int TRANSPARENT = -1;

int world::DAYLIGHT = 15; // TODO make this one change with the time of day

void attenuate(world::Chunk& chunk,int& last,int x,int y)
{
    const glm::ivec2 pos(x,y);
    world::Block block = chunk.getBlock(pos);
    if (block.isEmpty())
    {
        block.luminance = world::DAYLIGHT;
        last = world::DAYLIGHT;
    }
    else if (block.isTransparent())
    {
        if (block.luminance < last)
            block.luminance = last;
        else last = block.luminance;
        last += TRANSPARENT;
    }
    else
    {
        if (block.luminance < last)
            block.luminance = last;
        else last = block.luminance;
        last += SOLID;
    }
    if (last < 0)
        last = 0;
    chunk.setBlock(pos,block);
}

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

            attenuate(chunk,last,x,y);
        }
    }
    // BOTTOM to TOP
    for (auto x = 0; x < CHUNK_SIZE; ++x)
    {
        int last = DAYLIGHT;
        // compute top light
        if (surroundings[3].blocks[x] != 0)
        {
            const Block top = surroundings[3].getBlock(glm::ivec2(x,0));
            if (top.luminance == 0)
            {
                for (auto y = 1; y < CHUNK_SIZE; ++y)
                {
                    Block block = surroundings[3].getBlock(glm::ivec2(x,y));
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
            } else if (top.isEmpty())
                last = top.luminance;
            else if (top.isTransparent())
                last = top.luminance + TRANSPARENT;
            else
                last = top.luminance + SOLID;
        }
        // TOP to DOWN attenuation
        for (auto y = CHUNK_SIZE - 1; y >= 0; --y)
        {
            attenuate(chunk,last,x,y);
        }
    }
    // LEFT to RIGHT attenuation
    for (auto y = 0; y < CHUNK_SIZE; ++y)
    {
        int last = DAYLIGHT;

        const Block side = surroundings[1].getBlock(glm::ivec2(CHUNK_SIZE - 1,y));
        if (side.luminance == 0)
        {
            for (auto x = 2; x <= CHUNK_SIZE; ++x)
            {
                Block block = surroundings[1].getBlock(glm::ivec2(CHUNK_SIZE - x,y));
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
        } else if (side.isEmpty())
            last = side.luminance;
        else if (side.isTransparent())
            last = side.luminance + TRANSPARENT;
        else
            last = side.luminance + SOLID;

        for (auto x = 0; x < CHUNK_SIZE; ++x)
        {
            attenuate(chunk,last,x,y);
        }
    }
    // Right to Left
    for (auto y = 0; y < CHUNK_SIZE; ++y)
    {
        int last = DAYLIGHT;

        const Block side = surroundings[2].getBlock(glm::ivec2(0,y));
        if (side.luminance == 0)
        {
            for (auto x = 1; x < CHUNK_SIZE; ++x)
            {
                Block block = surroundings[2].getBlock(glm::ivec2(x,y));
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
        } else if (side.isEmpty())
            last = side.luminance;
        else if (side.isTransparent())
            last = side.luminance + TRANSPARENT;
        else
            last = side.luminance + SOLID;

        for (auto x = CHUNK_SIZE - 1; x >= 0; --x)
        {
            attenuate(chunk,last,x,y);
        }
    }
}