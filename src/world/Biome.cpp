#include "Biome.h"
#include "../Util.h"

#include <algorithm>

using namespace world;

Biome* world::biome = nullptr;

int Biome::getSurface(const float x)
{
	return static_cast<int>(Util::terrain_noise.noise(static_cast<float>(x) * getNoiseScale()) * getAmplifier());
}

MATERIAL Biome::getMaterial(const int depth)
{
	if (depth == 0) return GRASS_BLOCK;
	if (depth > 4) return STONE;
	return DIRT;
}

MATERIAL Biome::getCaveMaterial(const int depth)
{
	if (depth > 4) return STONE_WALL;
	return DIRT_WALL;
}

Block Biome::getPlant()
{
	return {EMPTY,{0,0},GRASS};
}

// OAK TREE CONSTANT PORTION
const Block world::OAK_TREE[] = {
	{OAK_LEAVES,{0,4},EMPTY},
	{OAK_LEAVES,{-1,3},EMPTY},{OAK_LEAVES,{0,3},EMPTY},{OAK_LEAVES,{1,3},EMPTY},
	{OAK_LEAVES,{-1,2},EMPTY},{OAK_LEAVES,{0,2},OAK_LOG},{OAK_LEAVES,{1,2},EMPTY},
	{OAK_LEAVES,{-1,1},EMPTY},{OAK_LEAVES,{0,1},OAK_LOG},{OAK_LEAVES,{1,1},EMPTY},
	{EMPTY,{0,0},OAK_LOG},
};

std::vector<Block> Forest::getTree()
{
	std::vector<Block> tree(std::begin(OAK_TREE), std::end(OAK_TREE));

	const int height = rand() % 3 + 2;

	std::transform(tree.begin(), tree.end(), tree.begin(),
		[height](Block& block) {
			block.position.y += height;
			return block;
		});

	tree.reserve(tree.size() + height + 1);

	for (int i = 0; i <= height; ++i) {
		tree.emplace_back(Block {EMPTY, {0, i}, OAK_LOG});
	}

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