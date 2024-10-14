#include "Chunk.h"

#include "Util.h"
#include "Storage.h"

#include <string>

using namespace world;

// The uint64_t are coloms the array is for rows
void Chunk::generate()
{
	glm::ivec2 blockPos = position * CHUNK_SIZE;
	for (auto x = 0; x < CHUNK_SIZE; x++)
	{
		int surface = static_cast<int>(Util::noise(static_cast<float>(blockPos.x + x) * biome->smoothness) * biome->min_max);
		int relative = blockPos.y - surface;
		blocks[x] = 0;
		if (relative >= CHUNK_SIZE) continue;
		if (relative < 0) relative = 0;
		for (int y = relative; y <= CHUNK_SIZE - 1; y++)
		{
			blocks[x] |= static_cast<uint64_t>(biome->getMaterial(surface + y - blockPos.y)) << (y * 8);
		}
	}
}

void Chunk::setBlock(MATERIAL material, glm::ivec2 position)
{
	// clear
	blocks[position.x] &= ~(0xFFULL << (position.y * 8));
	// set
	blocks[position.x] |= static_cast<uint64_t>(material) << (position.y * 8);
}

MATERIAL Chunk::getBlock(glm::ivec2 position) const
{
	return static_cast<MATERIAL>((blocks[position.x] >> (position.y * 8)) & 0xFF);
}