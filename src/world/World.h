#pragma once

#include "Region.h"
#include "../glew.h"

namespace world 
{
	constexpr int WORLD_SEED = 4563456;
	constexpr float PIXEL_SIZE = 16.0f;

	extern RegionHandler handler;
	extern glm::vec2 origin;

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

		static inline glm::ivec2 ToChunkSpace(glm::vec2 pos);

		static inline bool ChunkToArray(glm::ivec2& chunk);
		static inline void ArrayToChunk(glm::ivec2& chunk);
		static inline void GlobalToChunk(glm::ivec2& position, glm::ivec2& chunk);
		static inline void ChunkToGlobal(glm::ivec2& position,glm::ivec2 chunk);
	};
}
