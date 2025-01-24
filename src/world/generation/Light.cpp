#include "Generation.h"

#include <iostream>

// these number get interpreted as negative except for 0 which is interpreted as daylight
constexpr int SOLID = -3;
constexpr int TRANSPARENT = -1;

int world::DAYLIGHT = 15; // TODO make this one change with the time of day

void attenuate(world::Chunk& chunk,int& last,int x,int y)
{
    const glm::ivec2 pos(x,y);
    const world::Block block = chunk.getBlock(pos);
    int luminance = block.getLuminance();
    if (block.isEmpty())
    {
        luminance = world::DAYLIGHT;
        last = world::DAYLIGHT;
    }
    else if (block.isTransparent())
    {
        if (luminance < last)
            luminance = last;
        else last = luminance;
        last += TRANSPARENT;
    }
    else
    {
        if (luminance < last)
            luminance = last;
        else last = luminance;
        last += SOLID;
    }
    if (last < 0)
        last = 0;
    chunk.setLight(pos,luminance);
}

void world::Generate::Lighting(Chunk& chunk)
{
    // Get surrounding chunks
    glm::ivec2 top = chunk.position;
    top.y += 1;
    glm::ivec2 bottom = chunk.position;
    bottom.y -= 1;
    glm::ivec2 left = chunk.position;
    left.x -= 1;
    glm::ivec2 right = chunk.position;
    right.x += 1;

    Chunk surroundings[4];
    surroundings[0].position = top;
    handler.fetch(surroundings[0],false);
    surroundings[1].position = left;
    handler.fetch(surroundings[1],false);
    surroundings[2].position = right;
    handler.fetch(surroundings[2],false);
    surroundings[3].position = bottom;
    handler.fetch(surroundings[3],false);

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
            if (bottom.getLuminance() == 0)
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
                last = bottom.getLuminance();
            else if (bottom.isTransparent())
                last = bottom.getLuminance() + TRANSPARENT;
            else
                last = bottom.getLuminance() + SOLID;
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
            if (top.getLuminance() == 0)
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
                last = top.getLuminance();
            else if (top.isTransparent())
                last = top.getLuminance() + TRANSPARENT;
            else
                last = top.getLuminance() + SOLID;
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
        if (side.getLuminance() == 0)
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
            last = side.getLuminance();
        else if (side.isTransparent())
            last = side.getLuminance() + TRANSPARENT;
        else
            last = side.getLuminance() + SOLID;

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
        if (side.getLuminance() == 0)
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
            last = side.getLuminance();
        else if (side.isTransparent())
            last = side.getLuminance() + TRANSPARENT;
        else
            last = side.getLuminance() + SOLID;

        for (auto x = CHUNK_SIZE - 1; x >= 0; --x)
        {
            attenuate(chunk,last,x,y);
        }
    }
}