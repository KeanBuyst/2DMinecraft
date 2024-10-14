#pragma once

#include "Region.h"

#include <glew.h>

namespace world 
{
	constexpr int WORLD_SEED = 4563456;

	constexpr float WORLD_SIZE = 10.0f; // since we want a block postion to represent 1. 1/10 = 0.1
	constexpr int WORLD_WIDTH = 3; // min is 3x3
	constexpr int WORLD_HEIGHT = 3;

	extern RegionHandler handler;
	extern glm::vec2 origin;

	class World
	{
	public:
		~World();

		void init();
		void render();

		Block getBlock(glm::ivec2 position);
		void setBlock(Block block);
		// MetaBlock getMetaBlock();
	private:
		Chunk chunks[WORLD_WIDTH][WORLD_HEIGHT];
		GLuint VBO{}, VAO{};
		glm::ivec2 chunkOrigin{};

		inline void update_chunks();
		inline void generate(int x, int y);

		static inline glm::ivec2 ToChunkSpace(glm::ivec2 pos);

		static inline bool ToArray(glm::ivec2& chunk);
		static inline void FromArray(glm::ivec2& chunk);
		static inline void ToLocal(glm::ivec2& position, glm::ivec2& chunk);
		static inline void ToGlobal(glm::ivec2& position,glm::ivec2 chunk);
	};
}
