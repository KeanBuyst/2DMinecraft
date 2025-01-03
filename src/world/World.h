#pragma once

#include "Region.h"
#include "../glew.h"

namespace world 
{
	constexpr int WORLD_SEED = 4563456;

	extern RegionHandler handler;
	extern glm::vec2 origin;

	class World
	{
	public:
		~World();

		void init();
		void render();

		[[nodiscard]] Block getBlock(glm::ivec2 position, bool force = false) const;
		void setBlock(Block block, bool force = false);
		// MetaBlock getMetaBlock();
	private:
		Chunk chunks[WORLD_WIDTH][WORLD_HEIGHT];
		GLuint VBO{}, VAO{};
		glm::ivec2 chunkOrigin{};

		inline void update_chunks();
		inline void generate(int x, int y);

		static inline glm::ivec2 ToChunkSpace(glm::ivec2 pos);

		static inline bool ChunkToArray(glm::ivec2& chunk);
		static inline void ArrayToChunk(glm::ivec2& chunk);
		static inline void GlobalToChunk(glm::ivec2& position, glm::ivec2& chunk);
		static inline void ChunkToGlobal(glm::ivec2& position,glm::ivec2 chunk);
	};
}
