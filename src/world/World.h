#pragma once

#include "Region.h"
#include "../glew.h"

namespace world 
{
	constexpr int WORLD_SEED = 4563456;
	constexpr float PIXEL_SIZE = 16.0f;

	extern RegionHandler handler;

	class World
	{
	public:
		~World();

		void init();
		void render();

		[[nodiscard]] Block getBlock(glm::vec2 position, bool force = false) const;
		void setBlock(Block block, bool force = false);

		Block getSurfaceBlock(glm::vec2 position);
		// MetaBlock getMetaBlock();
	private:
		Chunk chunks[WORLD_WIDTH][WORLD_HEIGHT];
		GLuint VBO{}, VAO{};
		glm::ivec2 chunkOrigin{};

		inline void update_chunks();
		inline void GetChunk(int x, int y);
	};
}
