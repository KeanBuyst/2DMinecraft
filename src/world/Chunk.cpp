#include "Chunk.h"

#include "../Util.h"
#include "../resources/Storage.h"
#include "generation/Generation.h"

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
				glm::vec2 pos(blockPos.x + x_fix,blockPos.y + y_fix);
				data = GetWallOf(biome->getMaterial(relative)) << 8;
				if (!Generate::CaveGeneration(pos,relative))
				{
					BlockType ore;
					if (Generate::OreGeneration(pos,relative,ore))
						data |= static_cast<uint32_t>(ore);
					else data |= biome->getMaterial(relative);
				}
			}
			else data = 0;
		}
	}
}

bool Chunk::contains(const glm::vec2& globalPos) const
{
	return ToChunkSpace(globalPos) == position;
}

void Chunk::setBlock(const glm::ivec2 pos,const Block &block)
{
	uint32_t& data = GetData(pos.y,pos.x);

	data = (data & 0x00FF0000u) | (block.getRaw() & 0xFF00FFFFu);
}

void Chunk::setLightLevel(const glm::ivec2 pos,const int luminance)
{
	uint32_t& data = GetData(pos.y,pos.x);
	data &= 0xFFF0FFFFu;
	data |= luminance << 16;
}

void Chunk::setBlockBorder(const glm::ivec2 pos, const uint32_t faces)
{
	uint32_t& data = GetData(pos.y,pos.x);
	data &= 0xFF0FFFFFu;
	data |= faces << 20;
}

Block Chunk::getBlock(const glm::ivec2 pos)
{
	glm::ivec2 blockPos = pos;
	ChunkToGlobal(blockPos,position);
	return {blockPos,GetData(pos.y,pos.x)};
}

int Chunk::getLightLevel(const glm::ivec2 pos)
{
	const uint32_t& data = GetData(pos.y,pos.x);
	return static_cast<int>((data & 0x000F0000u) >> 16);
}

uint32_t& Chunk::GetData(const int y, const int x)
{
	// make x indicate the rows to improve memory efficiency during generation due to
	// x being the first iteration in generation.
	if (y < 0 || y >= CHUNK_SIZE || x < 0 || x >= CHUNK_SIZE)
		throw std::out_of_range("Chunk::GetData(y: " +
			std::to_string(y) + ",x: " +
			std::to_string(x) + ") is out of chunk bounds");
	return blocks[x * CHUNK_SIZE + y];
}
