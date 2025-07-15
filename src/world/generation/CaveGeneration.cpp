#include "Generation.h"

#include "../../Util.h"

bool world::Generate::CaveGeneration(glm::vec2 position,int depth)
{
    static constexpr int min_depth = 8;

    if (depth < min_depth) return false;

    static constexpr float size_x = 25.0f;
    static constexpr float size_y = 15.0f;

    return powf(Util::cave_noise.noise(position.x / size_x,position.y / size_y),0.5f) > 0.4f;
}