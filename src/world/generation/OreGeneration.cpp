#include "Generation.h"
#include "../../Util.h"

inline bool shouldSpawn(const glm::vec2 position, const float clump_size, const glm::vec2 offset)
{
    return Util::terrain_noise.noise((position.x + offset.x) / clump_size,(position.y + offset.y) / clump_size) >= 0.8f;
}

bool world::Generate::OreGeneration(const glm::vec2 position, const int depth,BlockType &ore)
{
    static const std::array<Util::ProbabilitySet<BlockType>,7> set = {
        Util::ProbabilitySet {COAL_ORE,60.0f},
        Util::ProbabilitySet {IRON_ORE,30.0f},
        Util::ProbabilitySet {COPPER_ORE,50.0f},
        Util::ProbabilitySet {GOLD_ORE,15.0f},
        Util::ProbabilitySet {LAPIS_ORE,10.0f},
        Util::ProbabilitySet {DIAMOND_ORE,5.0f},
        Util::ProbabilitySet {EMERALD_ORE, 2.0f},
    };
    static Util::ProbabilityGroup probability_group(set);

    // cascading switch statement to prevent scattered veins.
    // ordered by rarity
    switch (probability_group.get())
    {
    case EMERALD_ORE:
        if (depth >= 5)
            if (shouldSpawn(position,1.0f,{-56.0f,-19.0f}))
            {
                ore = EMERALD_ORE;
                return true;
            }
    case DIAMOND_ORE:
        if (depth >= 60)
            if (shouldSpawn(position,5.0f,{88.0f,98.0f}))
            {
                ore = DIAMOND_ORE;
                return true;
            }
    case LAPIS_ORE:
        if (depth >= 40)
            if (shouldSpawn(position,3.0f,{15.0f,14.0f}))
            {
                ore = LAPIS_ORE;
                return true;
            }
    case GOLD_ORE:
        if (depth >= 60)
            if (shouldSpawn(position,10.0f,{-70.0f,-34.0f}))
            {
                ore = GOLD_ORE;
                return true;
            }
    case IRON_ORE:
        if (depth >= 10)
            if (shouldSpawn(position,10.0f,{-40,30}))
            {
                ore = IRON_ORE;
                return true;
            }
    case COPPER_ORE:
        if (depth >= 5)
            if (shouldSpawn(position,30.0f,{23,-62}))
            {
                ore = COPPER_ORE;
                return true;
            }
    case COAL_ORE:
        if (depth >= 5 && depth <= 50)
            if (shouldSpawn(position,15.0f,{0,0}))
            {
                ore = COAL_ORE;
                return true;
            }
    }
    return false;
}
