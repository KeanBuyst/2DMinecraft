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
		[[nodiscard]] Block getBlock(glm::ivec2 pos);
		[[nodiscard]] int getLightLevel(glm::ivec2 pos);
		void setBlock(glm::ivec2 pos, const Block &block);
		void setLightLevel(glm::ivec2 pos,int luminance);

		void generate();

		[[nodiscard]] bool contains(const glm::vec2& globalPos) const;

		uint32_t blocks[CHUNK_SIZE*CHUNK_SIZE] = { 0 };
		uint8_t flag;

		glm::ivec2 position;
	private:
		inline uint32_t& GetData(int y, int x);
	};
}

