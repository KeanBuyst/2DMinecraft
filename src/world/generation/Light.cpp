#include "Generation.h"

#include <thread>
#include <iostream>

// these number get interpreted as negative except for 0 which is interpreted as daylight
constexpr int SOLID = -2;
constexpr int WALL = -1;

short world::DAYLIGHT = 15; // TODO make this one change with the time of day

class LightMap
{
private:
    static short GetInterference(const world::Block& block)
    {
        if (block.isTransparent())
            return WALL;
        if (block.getType() != world::EMPTY)
            return SOLID;
        if (block.getWall() != world::EMPTY)
             return WALL;
        return world::DAYLIGHT;
    }
public:
    // source chunks and neighboring chunks
    static constexpr int SIZE = world::CHUNK_SIZE * 3;
    short lightMap[SIZE][SIZE] = { 0 };
    short interferenceMap[SIZE][SIZE] = { 0 };

    explicit LightMap(world::Chunk chunks[3][3])
    {
        for (int y = 0; y < SIZE; ++y)
        {
            for (int x = 0; x < SIZE;++x)
            {
                const glm::ivec2 pos = {x % world::CHUNK_SIZE,y % world::CHUNK_SIZE};
                // ToChunkPosition not required here since never less than zero
                const int chunk_x = x / world::CHUNK_SIZE;
                const int chunk_y = y / world::CHUNK_SIZE;
                world::Chunk& chunk = chunks[chunk_x][chunk_y];
                world::Block block = chunk.getBlock(pos);

                // if (chunk_x != 1 && chunk_y != 1)
                // {
                //     lightMap[x][y] = static_cast<short>(chunk.getLightLevel(pos));
                // }

                {
                    short& interference = interferenceMap[x][y];
                    short iBlock = GetInterference(block);
                    if (interference > 0) interference = interference < iBlock ? iBlock : interference;
                    else interference = iBlock;
                }

                if (block.isEmpty())
                {
                    const glm::ivec2 left(x - 1,y);
                    if (bounds(left))
                    {
                        short& interference = interferenceMap[left.x][left.y];
                        interference = interference < world::DAYLIGHT ? world::DAYLIGHT : interference;
                    }
                    const glm::ivec2 right(x + 1,y);
                    if (bounds(right))
                    {
                        short& interference = interferenceMap[right.x][right.y];
                        interference = interference < world::DAYLIGHT ? world::DAYLIGHT : interference;
                    }
                    const glm::ivec2 bottom(x,y + 1);
                    if (bounds(bottom))
                    {
                        short& interference = interferenceMap[bottom.x][bottom.y];
                        interference = interference < world::DAYLIGHT ? world::DAYLIGHT : interference;
                    }
                    const glm::ivec2 top(x,y - 1);
                    if (bounds(top))
                    {
                        short& interference = interferenceMap[top.x][top.y];
                        interference = interference < world::DAYLIGHT ? world::DAYLIGHT : interference;
                    }
                }
            }
        }
    }

    static bool bounds(const glm::ivec2& pos)
    {
        return pos.x >= 0 && pos.x < LightMap::SIZE && pos.y >= 0 && pos.y < LightMap::SIZE;
    }

    [[nodiscard]] std::unique_ptr<short[]> getLightMap(const int chunk_x, const int chunk_y) const
    {
        auto map = std::make_unique<short[]>(world::CHUNK_SIZE * world::CHUNK_SIZE);
        const int pos_x = chunk_x * world::CHUNK_SIZE;
        // inverses map along Y so that 0,0 is bottom left instead of top left
        const int pos_y = (2 - chunk_y) * world::CHUNK_SIZE;
        for (int x = 0; x < world::CHUNK_SIZE; ++x)
        {
            for (int y = 0; y < world::CHUNK_SIZE; ++y)
            {
                map[x * world::CHUNK_SIZE + y] = lightMap[pos_x + x][pos_y + y];
            }
        }
        return map;
    }
};

short level(const int lightLevel)
{
    if (lightLevel > 15) return 15;
    if (lightLevel < 0) return 0;
    return static_cast<short>(lightLevel);
}

enum SpreadType
{
    POINT,
    LINEAR,
    DIAGONAL
};

void spread(LightMap* map,glm::ivec2 pos, short lightLevel, const SpreadType type,glm::ivec2 direction = {0,0})
{
    // end case
    if (lightLevel == 0) return;
    // optimization
    if (map->lightMap[pos.x][pos.y] < lightLevel)
    {
        // set light level
        map->lightMap[pos.x][pos.y] = lightLevel;
    }
    // propagate
    switch (type)
    {
    case POINT:
        {
            glm::ivec2 dir(-1,1);
            glm::ivec2 next = pos + dir;
            if (LightMap::bounds(next)) spread(
                map,
                next,
                level(lightLevel + map->interferenceMap[next.x][next.y]),
                DIAGONAL,
                dir
                );
            dir = {0,-1};
            next = pos + dir;
            if (LightMap::bounds(next)) spread(
                map,
                next,
                level(lightLevel + map->interferenceMap[next.x][next.y]),
                LINEAR,
                dir
                );
            dir = {1,1};
            next = pos + dir;
            if (LightMap::bounds(next)) spread(
                map,
                next,
                level(lightLevel + map->interferenceMap[next.x][next.y]),
                DIAGONAL,
                dir
                );
            dir = {-1,0};
            next = pos + dir;
            if (LightMap::bounds(next)) spread(
                map,
                next,
                level(lightLevel + map->interferenceMap[next.x][next.y]),
                LINEAR,
                dir
                );
            dir = {1,0};
            next = pos + dir;
            if (LightMap::bounds(next)) spread(
                map,
                next,
                level(lightLevel + map->interferenceMap[next.x][next.y]),
                LINEAR,
                dir
                );
            dir = {-1,-1};
            next = pos + dir;
            if (LightMap::bounds(next)) spread(
                map,
                next,
                level(lightLevel + map->interferenceMap[next.x][next.y]),
                DIAGONAL,
                dir
                );
            dir = {0,1};
            next = pos + dir;
            if (LightMap::bounds(next)) spread(
                map,
                next,
                level(lightLevel + map->interferenceMap[next.x][next.y]),
                LINEAR,
                dir
                );
            dir = {1,-1};
            next = pos + dir;
            if (LightMap::bounds(next)) spread(
                map,
                next,
                level(lightLevel + map->interferenceMap[next.x][next.y]),
                DIAGONAL,
                dir
                );
            break;
        }
    case LINEAR:
        {
            glm::ivec2 next = pos + direction;
            if (!LightMap::bounds(next)) return;
            spread(map,next, level(lightLevel + map->interferenceMap[next.x][next.y]),LINEAR,direction);
        }
        break;
    case DIAGONAL:
        {
            glm::ivec2 next = pos + direction;
            if (LightMap::bounds(next)) spread(map,next, level(lightLevel + map->interferenceMap[next.x][next.y]),DIAGONAL,direction);
            glm::ivec2 dir = {direction.x,0};
            next = pos + dir;
            if (LightMap::bounds(next)) spread(map,next, level(lightLevel + map->interferenceMap[next.x][next.y]),LINEAR,dir);
            dir = {0,direction.y};
            next = pos + dir;
            if (LightMap::bounds(next)) spread(map,next, level(lightLevel + map->interferenceMap[next.x][next.y]),LINEAR,dir);
        }
        break;
    }
}

void propagation(world::World* world,world::Chunk neighbors[3][3], const glm::ivec2 position)
{
    auto* map = new LightMap(neighbors);
    for (auto x = 0; x < LightMap::SIZE; ++x)
    {
        for (auto y = 0; y < LightMap::SIZE; ++y)
        {
            if (map->interferenceMap[x][y] > 0)
            {
                spread(map,{x,y},map->interferenceMap[x][y],POINT);
            }
        }
    }

    world->setLightMap(position,map->getLightMap(1,1));
    delete map;
}

void world::Generate::Lighting(World* world, const glm::ivec2 chunk_pos)
{
    Chunk neighbors[3][3];
    // get neighboring chunks
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy) {
            auto& c = neighbors[dx+1][dy+1];
            c.position = chunk_pos + glm::ivec2(dx, dy);
            c = world->getChunk(c.position);
        }
    }
    // Propagation
    propagation(world,neighbors,chunk_pos);
}