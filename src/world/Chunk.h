#pragma once

#include "Biome.h"

namespace world 
{
	// chunk is 8x8 blocks
	// 1 = 0.0625 OpenGL choords = one block size at the scale I want.

	class Chunk
	{
	public:
		// Chunk local choords
		[[nodiscard]] Block getBlock(glm::ivec2 position) const;
		void setBlock(glm::ivec2 pos, const Block &block);

		void generate();

		uint64_t blocks[CHUNK_SIZE] = { 0 };
		uint32_t lightMap[CHUNK_SIZE] = { 0 }; // (0-15) per block
		uint8_t depth[CHUNK_SIZE] = { 0 }; // 0 = foreground & 1 = background

		glm::ivec2 position;
	private:
		// std::unordered_map<glm::uvec2, Metadata> blockData;
	};
}

