#pragma once

#include "Region.h"

#include <memory>
#include <SDL3/SDL_gpu.h>

namespace world 
{
	constexpr int WORLD_SEED = 4563456;
	extern RegionHandler handler;

	struct StaticTerrainRenderInfo
	{
		SDL_GPUBuffer* shared_vertices;
		SDL_GPUBuffer* indirect_buffer;

		SDL_GPUTexture* input_buffer;

		SDL_GPUComputePipeline* compute_pipeline;
		SDL_GPUGraphicsPipeline* graphics_pipeline;
	};

	class World
	{
	public:
		static void Init();
		static void Cleanup();

		World();
		~World();

		void render(SDL_GPUCommandBuffer* cmd,SDL_GPUTexture* swapChain, uint32_t width, uint32_t height);

		// all positions inputted are world (global) positions
		[[nodiscard]] Block getBlock(glm::vec2 position) const;
		void setBlock(const Block& block,bool doPostUpdate = false);
		// Chunk local position
		[[nodiscard]] Chunk getChunk(glm::ivec2 position) const;
		void setChunk(const Chunk &chunk);

		void setLightMap(glm::ivec2 chunk_pos,std::unique_ptr<short[]> lightMap);

		// MetaBlock getMetaBlock();
	private:
		static StaticTerrainRenderInfo render_info;

		Chunk chunks[WORLD_WIDTH][WORLD_HEIGHT];
		bool update;

		inline void update_chunks();
		inline void GetChunk(int x, int y,glm::ivec2 current);

		void post_generation(Chunk& chunk);
	};

	extern World* world_ptr;
}
