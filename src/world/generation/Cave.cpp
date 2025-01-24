#include <iostream>

#include "Generation.h"
#include "../../Util.h"

constexpr int MAX_CAVE_SIZE = 8;
constexpr int MIN_CAVE_SIZE = 2;

constexpr int MAX_CAVE_LENGTH = 150;
constexpr int MIN_CAVE_LENGTH = 32;

void world::Generate::Cave(World* world, const int x)
{
    glm::vec2 positions[MAX_CAVE_SIZE];
    glm::vec2 direction(0,-1);
    const int dynamic_size = MIN_CAVE_SIZE + rand() % (MAX_CAVE_SIZE - MIN_CAVE_SIZE + 1);
    const int dynamic_length = MIN_CAVE_LENGTH + rand() % (MAX_CAVE_LENGTH - MIN_CAVE_LENGTH + 1);

    // set positions to correct starting positions on the surface
    for (auto i = 0; i < dynamic_size; ++i)
    {
        // retrieve surface location
        // TODO check biome at coords
        positions[i].x = x + i;
        positions[i].y = floorf(Util::noise(positions[i].x * biome->smoothness) * biome->min_max);
    }
    // compute cave etching cycles
    for (auto cycle = 0; cycle < dynamic_length; ++cycle)
    {
        // compute a direction (Perlin worms. Constrained)
        direction.x = glm::clamp(Util::noise(positions[0].x / 1000.0f) + direction.x,-1.0f,1.0f);
        std::cout << direction.x << std::endl;
        direction.y = glm::clamp(Util::noise(positions[0].y / 1000.0f) + direction.y,-1.0f,1.0f);

        for (auto i = 0; i < dynamic_size; ++i)
        {
            Block block(EMPTY,glm::round(positions[i]),EMPTY);
            world->setBlock(block);
            positions[i] += direction;
        }
    }
}
