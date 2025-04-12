#include "Chunk.h"

#include "../Util.h"
#include "../resources/Storage.h"

#include <iostream>

using namespace world;

// The uint64_t are coloms the array is for rows
void Chunk::generate()
{
	glm::ivec2 blockPos(0,0);
	ChunkToGlobal(blockPos,position);
	for (auto x = 0; x < CHUNK_SIZE; ++x)
	{
		// prevent mirroring in x dimension
		const int x_fix = position.x < 0 ? (CHUNK_SIZE - 1) - x : x;

		// calculate surface
		const int surface = biome->getSurface(static_cast<float>(blockPos.x + x_fix));

		for (auto y = 0; y < CHUNK_SIZE; ++y)
		{
			// prevent mirroring in y dimension
			const int y_fix = position.y < 0 ? (CHUNK_SIZE - 1) - y : y;

			// calculate relative surface
			const int relative = surface - (blockPos.y + y_fix);

			uint32_t& data = GetData(y_fix,x_fix);

			if (relative >= 0)
			{
				data = static_cast<uint32_t>(biome->getMaterial(relative));
			}
			else data = 0;
		}
	}
}

void Chunk::setBlock(const glm::ivec2 pos,const Block &block)
{
	GetData(pos.y,pos.x) = block.getRaw();
}

void Chunk::setLight(const glm::ivec2 pos,const int luminance)
{
	uint32_t& data = GetData(pos.y,pos.x);
	data &= 0xFFF0FFFFu;
	data |= luminance << 16;
}

Block Chunk::getBlock(const glm::ivec2 pos)
{
	glm::ivec2 blockPos = pos;
	ChunkToGlobal(blockPos,position);
	return {blockPos,GetData(pos.y,pos.x)};
}

uint32_t& Chunk::GetData(const int y, const int x)
{
	// make x indicate the rows to improve memory efficiency during generation due to
	// x being the first iteration in generation.
	if (y < 0 || y >= CHUNK_SIZE || x < 0 || x >= CHUNK_SIZE)
		throw std::out_of_range("Chunk::GetData(y,x) is out of chunk bounds");
	return blocks[x * CHUNK_SIZE + y];
}
