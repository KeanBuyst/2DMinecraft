#include "Biome.h"
#include "../Util.h"

using namespace world;

Biome* world::biome = nullptr;

MATERIAL Biome::getPlant()
{
	return GRASS;
}

int Biome::getSurface(const float x)
{
	return static_cast<int>(Util::noise(static_cast<float>(x) * getNoiseScale()) * getAmplifier());
}

MATERIAL Biome::getMaterial(const int depth)
{
	if (depth == 0) return GRASS_BLOCK;
	if (depth > 4) return STONE;
	return DIRT;
}

std::vector<Block> Forest::getTree()
{
	std::vector<Block> tree;
	return tree;
}

float Forest::getNoiseScale()
{
	return 0.2f;
}

float Forest::getAmplifier()
{
	return 5.0f;
}