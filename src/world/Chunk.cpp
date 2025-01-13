#include "Chunk.h"

#include "../Util.h"
#include "../resources/Storage.h"

#include <iostream>

using namespace world;

// The uint64_t are coloms the array is for rows
void Chunk::generate()
{
	const glm::ivec2 blockPos = position * CHUNK_SIZE;
	for (auto x = 0; x < CHUNK_SIZE; x++)
	{
		// reset all values
		blocks[x] = 0;
		walls[x] = 0;

		// calculate surface
		const int surface = static_cast<int>(Util::noise(static_cast<float>(blockPos.x + x) * biome->smoothness) * biome->min_max);

		// calculate relative surface
		int relative = blockPos.y - surface;
		if (relative >= CHUNK_SIZE) continue; // surface below chunk
		if (relative < 0)
		{
			// surface above chunk
			relative = 0;
		}

		for (int y = relative; y <= CHUNK_SIZE - 1; y++)
		{
			blocks[x] |= static_cast<uint64_t>(biome->getMaterial(surface + y - blockPos.y)) << (y * 8);
		}
	}
}

void Chunk::setBlock(const glm::ivec2 pos,const Block &block)
{
	// clear
	blocks[pos.x] &= ~(0xFFULL << (pos.y * 8));
	walls[pos.x] &= ~(0xFFULL << (pos.y * 8));
	lightMap[pos.x] &= ~(0xFU << (pos.y * 4));
	// set
	if (block.type != EMPTY) blocks[pos.x] |= static_cast<uint64_t>(block.type) << (pos.y * 8);
	if (block.getWall() != EMPTY) walls[pos.x] |= static_cast<uint64_t>(block.getWall()) << (pos.y * 8);
	if (block.luminance != 0) lightMap[pos.x] |= static_cast<uint32_t>(block.luminance) << (pos.y * 4);
}

Block Chunk::getBlock(const glm::ivec2 position) const
{
	return {
		static_cast<MATERIAL>((blocks[position.x] >> (position.y * 8)) & 0xFFULL),
		position,
		static_cast<MATERIAL>((walls[position.x] >> (position.y * 8)) & 0xFFULL),
		static_cast<int>(lightMap[position.x] >> position.y * 4 & 0xF),
	};
}
