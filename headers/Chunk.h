#pragma once

#include "Biome.h"
#include <unordered_map>


namespace world 
{
	// chunk is 8x8 blocks
	// 1 = 0.0625 OpenGL choords = one block size at the scale I want.
	constexpr int CHUNK_SIZE = 8;

	class Chunk
	{
	public:
		// Chunk local choords
		MATERIAL getBlock(glm::ivec2 position) const;
		void setBlock(MATERIAL material,glm::ivec2 position);

		void generate();

		uint64_t blocks[CHUNK_SIZE] = { 0 };

		glm::ivec2 position;
	private:
		// std::unordered_map<glm::uvec2, Metadata> blockData;
	};
}

