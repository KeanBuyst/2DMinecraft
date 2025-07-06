#pragma once

#include "Region.h"
#include "../glew.h"

#include <memory>

namespace world 
{
	constexpr int WORLD_SEED = 4563456;

	extern RegionHandler handler;

	class World
	{
	public:
		~World();

		void init();
		void render();

		// all positions inputted are world (global) positions
		[[nodiscard]] Block getBlock(glm::vec2 position) const;
		void setBlock(const Block& block);
		// Chunk local position
		[[nodiscard]] Chunk getChunk(glm::ivec2 position) const;
		void setChunk(const Chunk &chunk);
		// Works asynchronously
		void setLightMap(glm::ivec2 chunk_pos,std::unique_ptr<short[]> lightMap);

		// MetaBlock getMetaBlock();
	private:
		Chunk chunks[WORLD_WIDTH][WORLD_HEIGHT];
		GLuint VBO{}, VAO{};

		inline void update_chunks();
		inline void GetChunk(int x, int y,glm::ivec2 current);

		void post_generation(Chunk& chunk);
	};
}
