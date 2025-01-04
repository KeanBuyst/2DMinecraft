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
		depth[x] = 0;
		lightMap[x] = 0;

		// calculate surface
		const int surface = static_cast<int>(Util::noise(static_cast<float>(blockPos.x + x) * biome->smoothness) * biome->min_max);

		// calculate relative surface
		int relative = blockPos.y - surface;
		uint32_t brightness = 0;
		if (relative >= CHUNK_SIZE) continue; // surface below chunk
		if (relative < 0)
		{
			// surface above chunk
			brightness = static_cast<uint32_t>(relative >= -5 ? 15 + relative * 3 : 0);
			relative = 0;
		}
		else
		{
			// surface within chunk
			brightness = 0xF;
		}

		for (int y = relative; y <= CHUNK_SIZE - 1; y++)
		{
			// add light
			lightMap[x] |= brightness << (y * 4);
			if (brightness != 0) brightness -= 3;
			blocks[x] |= static_cast<uint64_t>(biome->getMaterial(surface + y - blockPos.y)) << (y * 8);
		}
	}
}

void Chunk::setBlock(const glm::ivec2 pos,const Block &block)
{
	// clear
	blocks[pos.x] &= ~(0xFFULL << (pos.y * 8));
	// set
	blocks[pos.x] |= static_cast<uint64_t>(block.type) << (pos.y * 8);
}

Block Chunk::getBlock(const glm::ivec2 position) const
{
	return {
		static_cast<MATERIAL>((blocks[position.x] >> (position.y * 8)) & 0xFF),
		position,
		static_cast<Layer>(depth[position.x] >> position.y),
		static_cast<uint8_t>((lightMap[position.x] >> (position.y * 4) & 0xF)),
	};
}
