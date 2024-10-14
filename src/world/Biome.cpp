#include "Biome.h"

using namespace world;

Biome* world::biome = nullptr;

Biome::Biome(float smoothness, int min_max) : smoothness(smoothness), min_max(min_max)
{
	
}

MATERIAL Biome::getMaterial(int layer)
{
	if (layer == 0) return GRASS_BLOCK;
	if (layer > 4) return STONE;
	return DIRT;
}