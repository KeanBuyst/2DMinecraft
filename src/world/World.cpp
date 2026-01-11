#include "World.h"

#include <iostream>

#include "../Util.h"
#include "vector"
#include "../Application.h"
#include "../resources/Resources.h"
#include "generation/Generation.h"

using namespace world;

RegionHandler world::handler = RegionHandler();
World* world::world_ptr = nullptr;
StaticTerrainRenderInfo World::render_info;

// generated data
struct GeneratedVertex {
	float pos[4];
	float uv[2];
	float luminance;
	uint32_t border;
};

void World::Init()
{
	// vertex buffer
	SDL_GPUBufferCreateInfo bufferInfo = {
		.usage = SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_WRITE | SDL_GPU_BUFFERUSAGE_VERTEX,
		.size = sizeof(GeneratedVertex) * WORLD_WIDTH * WORLD_HEIGHT * CHUNK_SIZE * CHUNK_SIZE * 6 * 3,
		.props = 0
	};
	render_info.shared_vertices = SDL_CreateGPUBuffer(Application::GPU_DEVICE,&bufferInfo);

	// Input buffer
	SDL_GPUTextureCreateInfo inputInfo = {
		.type = SDL_GPU_TEXTURETYPE_2D,
		.format = SDL_GPU_TEXTUREFORMAT_R32_UINT,
		.usage =  SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_READ,
		.width = WORLD_WIDTH * CHUNK_SIZE,
		.height = WORLD_HEIGHT * CHUNK_SIZE,
		.layer_count_or_depth = 1,
		.num_levels = 1
	};
	render_info.input_buffer = SDL_CreateGPUTexture(Application::GPU_DEVICE,&inputInfo);

	// Indirect buffer
	SDL_GPUBufferCreateInfo indirectInfo = {
		.usage = SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_WRITE | SDL_GPU_BUFFERUSAGE_INDIRECT,
		.size = sizeof(SDL_GPUIndirectDrawCommand),
		.props = 0
	};
	render_info.indirect_buffer = SDL_CreateGPUBuffer(Application::GPU_DEVICE, &indirectInfo);

	// compute shader pipline
	render_info.compute_pipeline = gl::GetComputePipline("terrain_comp");
	// graphics shader pipline
	gl::Shader vertex("terrain_vert",gl::VERTEX);
	gl::Shader fragment("terrain_frag",gl::FRAGMENT);

	constexpr int num_attributes = 4;
	SDL_GPUVertexAttribute attributes[num_attributes] = {
		{0,0,SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,offsetof(GeneratedVertex,pos)},
		{1,0,SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,offsetof(GeneratedVertex,uv)},
		{2,0,SDL_GPU_VERTEXELEMENTFORMAT_FLOAT,offsetof(GeneratedVertex,luminance)},
		{3,0,SDL_GPU_VERTEXELEMENTFORMAT_UINT,offsetof(GeneratedVertex,border)}
	};

	SDL_GPUVertexBufferDescription vertexBindingDesc = {
		0,
		sizeof(GeneratedVertex),
		SDL_GPU_VERTEXINPUTRATE_VERTEX,
		0
	};

	SDL_GPUTextureFormat format = SDL_GetGPUSwapchainTextureFormat(Application::GPU_DEVICE, Application::WINDOW);

	if (format == SDL_GPU_TEXTUREFORMAT_INVALID)
	{
		SDL_Log("Error: Swapchain format is INVALID (0). Window not claimed?");
		throw "Error: Swapchain format is INVALID (0). Window not claimed?";
	}

	SDL_GPUColorTargetDescription targetDesc = {
		format,
		 {
		 	.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
		 	.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
		 	.color_blend_op = SDL_GPU_BLENDOP_ADD,
		 	.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
		 	.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
		 	.alpha_blend_op = SDL_GPU_BLENDOP_ADD,
		 	.enable_blend = true,
		 }
	};

	SDL_GPUGraphicsPipelineCreateInfo pipelineInfo = {
		.vertex_shader = vertex,
		.fragment_shader = fragment,
		.vertex_input_state = {
			.vertex_buffer_descriptions = &vertexBindingDesc,
			.num_vertex_buffers = 1,
			.vertex_attributes = attributes,
			.num_vertex_attributes = num_attributes
		},
		.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		.rasterizer_state = {
			.fill_mode = SDL_GPU_FILLMODE_FILL,
			.cull_mode = SDL_GPU_CULLMODE_NONE,
			.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE
		},
		.depth_stencil_state = {
			.compare_op = SDL_GPU_COMPAREOP_ALWAYS,
			.enable_depth_test = false,
			.enable_depth_write = false
		},
		.target_info = {
			.color_target_descriptions = &targetDesc,
			.num_color_targets = 1,
		}
	};

	SDL_GPUGraphicsPipeline* terrainPipline = SDL_CreateGPUGraphicsPipeline(Application::GPU_DEVICE,&pipelineInfo);
	if (!terrainPipline)
	{
		SDL_Log("Error: SDL_CreateGPUGraphicsPipeline -> %s", SDL_GetError());
		throw "Error: SDL_CreateGPUGraphicsPipeline";
	}
	render_info.graphics_pipeline = terrainPipline;
}

void World::Cleanup()
{
	SDL_ReleaseGPUGraphicsPipeline(Application::GPU_DEVICE,render_info.graphics_pipeline);
	SDL_ReleaseGPUComputePipeline(Application::GPU_DEVICE,render_info.compute_pipeline);

	SDL_ReleaseGPUTexture(Application::GPU_DEVICE,render_info.input_buffer);

	SDL_ReleaseGPUBuffer(Application::GPU_DEVICE,render_info.shared_vertices);
	SDL_ReleaseGPUBuffer(Application::GPU_DEVICE,render_info.indirect_buffer);
}


World::World() : update(true)
{
	// set up chunks for loading/generation
	Util::InitNoise();
	world::biome = new Forest();
	// setup of initial area
	for (auto wx = 0; wx < WORLD_WIDTH; wx++)
	{
		for (auto wy = 0; wy < WORLD_HEIGHT; wy++)
		{
			glm::ivec2 pos(wx, wy);
			ArrayToChunk(pos);
			Chunk& chunk = chunks[wx][wy];
			chunk.position = pos;
			handler.fetch(chunk);
		}
	}
	// post generation
	for (auto wx = POST_GEN_BUF; wx < WORLD_WIDTH - POST_GEN_BUF; wx++)
	{
		for (auto wy = POST_GEN_BUF; wy < WORLD_HEIGHT - POST_GEN_BUF; wy++)
		{
			Chunk& chunk = chunks[wx][wy];
			if (!(chunk.flag & POST_GENERATED))
			{
				post_generation(chunk);
			}
			Generate::Lighting(this,chunk.position);
		}
	}
}

World::~World()
{
	for (auto x = 0; x < WORLD_WIDTH; x++)
	{
		for (auto y = 0; y < WORLD_HEIGHT; y++)
		{
			handler.save(chunks[x][y]);
		}
	}
	delete biome;
}

void World::render(SDL_GPUCommandBuffer* cmd,SDL_GPUTexture* swapChain, uint32_t width, uint32_t height)
{
	// compile chunks
	constexpr int size = WORLD_WIDTH * WORLD_HEIGHT * CHUNK_SIZE * CHUNK_SIZE;
	static std::vector<uint32_t> tiles;
	tiles.reserve(size);
	tiles.clear();
	// load chunks for new position (if new position)
	update_chunks();

	if (update)
	{

		for (int y = 0; y < WORLD_HEIGHT * CHUNK_SIZE; ++y)
		{
			for (int x = 0; x < WORLD_WIDTH * CHUNK_SIZE; ++x)
			{
				int chunk_x = x / CHUNK_SIZE;
				int chunk_y = y / CHUNK_SIZE;
				int local_x = x % CHUNK_SIZE;
				int local_y = y % CHUNK_SIZE;

				Chunk& chunk = chunks[chunk_x][chunk_y];
				Block block = chunk.getBlock(glm::ivec2(local_x,local_y));
				// Only render wall if block is transparent
				if (!block.isTransparent() && block.getType() != EMPTY)
				{
					block.setWall(EMPTY);
				}

				tiles.push_back(block.getRaw());
			}
		}
		if (tiles.size() == 0)
		{
			SDL_Log("render tiles error\nShouldn't ever be less then WORLD_WIDTH * WORLD_HEIGHT * CHUNK_SIZE * CHUNK_SIZE");
			return;
		}

		SDL_GPUIndirectDrawCommand resetCmd = {0,1,0,0};

		uint32_t dataSize = size * sizeof(uint32_t);
		uint32_t IDDC_size = sizeof(SDL_GPUIndirectDrawCommand);

		uint32_t netSize = dataSize + IDDC_size;

		// buffer that is transported to GPU
		SDL_GPUTransferBufferCreateInfo transferInfo = {
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = netSize
		};
		SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(Application::GPU_DEVICE, &transferInfo);

		uint8_t* mapPtr = reinterpret_cast<uint8_t*>(SDL_MapGPUTransferBuffer(Application::GPU_DEVICE,transferBuffer,false));
		memcpy(mapPtr, tiles.data(), dataSize);
		memcpy(mapPtr + dataSize,&resetCmd,IDDC_size);
		SDL_UnmapGPUTransferBuffer(Application::GPU_DEVICE, transferBuffer);

		SDL_GPUTextureTransferInfo tileSrc = {
			transferBuffer,
			0
		};
		SDL_GPUTextureRegion tileDest = {
			.texture = render_info.input_buffer,
			.w = WORLD_WIDTH * CHUNK_SIZE,
			.h = WORLD_HEIGHT * CHUNK_SIZE,
			.d = 1
		};
		SDL_GPUTransferBufferLocation indirectSrc = {
			transferBuffer,
			 dataSize
		};
		SDL_GPUBufferRegion indirectDest = {
			render_info.indirect_buffer,
			0,
			IDDC_size
		};

		// upload data to GPU pass
		SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd);

		// compute & vertex & pixel shader data
		SDL_UploadToGPUTexture(copy_pass,&tileSrc,&tileDest,true);
		SDL_UploadToGPUBuffer(copy_pass,&indirectSrc,&indirectDest,true);

		SDL_EndGPUCopyPass(copy_pass);
		SDL_ReleaseGPUTransferBuffer(Application::GPU_DEVICE, transferBuffer);

		// Uniforms
		SDL_PushGPUComputeUniformData(cmd,0,&VIEW_SIZE,sizeof(glm::vec2));

		// specify storage buffer used by compute shader
		SDL_GPUStorageBufferReadWriteBinding WRBindings[] = {
			{ .buffer = render_info.shared_vertices, .cycle = true },
			{ . buffer = render_info.indirect_buffer, .cycle = false }

		};

		// start compute shader processing
		SDL_GPUComputePass* compute_pass = SDL_BeginGPUComputePass(
			cmd,
			NULL, 0,
			WRBindings, 2
		);
		SDL_BindGPUComputePipeline(compute_pass,render_info.compute_pipeline);

		SDL_BindGPUComputeStorageTextures(compute_pass,0,&render_info.input_buffer,1);

		constexpr uint32_t groupCountX = (WORLD_WIDTH * CHUNK_SIZE + 7) / 8;
		constexpr uint32_t groupCountY = (WORLD_HEIGHT * CHUNK_SIZE + 7) / 8;
		SDL_DispatchGPUCompute(compute_pass,groupCountX,groupCountY,1);

		SDL_EndGPUComputePass(compute_pass);

		update = false;
	}

	SDL_GPUColorTargetInfo color_target_info = {
		.texture = swapChain,
		.mip_level = 0,
		.layer_or_depth_plane = 0,
		.clear_color = {0.529f,0.8078f,0.9215686f,1.0f},
		.load_op = SDL_GPU_LOADOP_CLEAR,
		.store_op = SDL_GPU_STOREOP_STORE,
		.cycle = false
	};

	SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd,&color_target_info, 1, NULL);
	SDL_BindGPUGraphicsPipeline(render_pass,render_info.graphics_pipeline);

	SDL_GPUViewport viewport = {
		0, 0,
		 (float)width, (float)height,
		-1.0f,1.0f
	};
	SDL_SetGPUViewport(render_pass,&viewport);

	SDL_Rect scissor = {
		0,0,(int)width,(int)height
	};
	SDL_SetGPUScissor(render_pass,&scissor);

	SDL_GPUBufferBinding vertexBinding = {
		.buffer = render_info.shared_vertices,
		.offset = 0
	};
	SDL_BindGPUVertexBuffers(render_pass, 0, &vertexBinding, 1);

	SDL_GPUTextureSamplerBinding texture_binding = {
		.texture = *res::atlas::tiles,
		.sampler = gl::Texture::GetSampler()
	};
	SDL_BindGPUFragmentSamplers(render_pass,0,&texture_binding,1);

	// finally drawing the stuff
	SDL_DrawGPUPrimitivesIndirect(
		render_pass,
		render_info.indirect_buffer,
		0,
		1
	);

	SDL_EndGPURenderPass(render_pass);
}

void World::setBlock(const Block& block, const bool doPostUpdate)
{
	update = true;

	glm::ivec2 c_pos;
	// floor brings -0.5 to -1 and 0.5 to 0
	glm::ivec2 pos(floorf(block.position.x),floorf(block.position.y));
	GlobalToChunk(pos, c_pos);
	// Checks if chunk is part of currently loaded chunks
	if(glm::ivec2 a_pos = c_pos; !ChunkToArray(a_pos))
	{
		Chunk chunk;
		chunk.position = c_pos;
		handler.fetch(chunk);
		chunk.setBlock(pos,block);
		handler.save(chunk);
	}
	else
	{
		Chunk& chunk = chunks[a_pos.x][a_pos.y];
		// check if light update should accoure
		if (doPostUpdate)
		{
			if (chunk.getBlock(pos).getInterference() != block.getInterference())
			{
				chunk.setBlock(pos,block);

				static constexpr int radius = 1;

				const int min_x = std::max(a_pos.x - radius,0);
				const int min_y = std::max(a_pos.y - radius,0);

				const int max_x = std::min(a_pos.x + radius,WORLD_WIDTH - 1);
				const int max_y = std::min(a_pos.y + radius,WORLD_HEIGHT - 1);

				for (int x = min_x; x <= max_x; ++x)
				{
					for (int y = min_y; y <= max_y; ++y)
					{
						Generate::Lighting(this,chunks[x][y].position);
						Generate::BlockBorder(this,chunks[x][y].position);
					}
				}
			} else chunk.setBlock(pos,block);

			Generate::BlockBorder(this,chunk.position);

		} else chunk.setBlock(pos,block);
	}
}

Block World::getBlock(const glm::vec2 position) const
{
	// position relative to origin due to loaded chunks being around origin
	glm::ivec2 c_pos;
	glm::ivec2 pos(floorf(position.x),floorf(position.y));
	GlobalToChunk(pos, c_pos);
	return getChunk(c_pos).getBlock(pos);
}

Chunk World::getChunk(const glm::ivec2 position) const
{
	if(glm::ivec2 a_pos = position; !ChunkToArray(a_pos))
	{
		Chunk chunk;
		chunk.position = position;
		handler.fetch(chunk);
		return chunk;
	}
	else
	{
		return chunks[a_pos.x][a_pos.y];
	}
}

void World::setChunk(const Chunk& chunk)
{
	update = true;

	if(glm::ivec2 a_pos = chunk.position; !ChunkToArray(a_pos))
	{
		handler.save(chunk);
	}
	else
	{
		chunks[a_pos.x][a_pos.y] = chunk;
	}
}

void World::setLightMap(const glm::ivec2 chunk_pos,std::unique_ptr<short[]> lightMap)
{
	update = true;

	if(glm::ivec2 a_pos = chunk_pos; !ChunkToArray(a_pos))
	{
		Chunk chunk;
		chunk.position = chunk_pos;
		handler.fetch(chunk);
		for (auto x = 0; x < CHUNK_SIZE; ++x)
		{
			for (auto y = 0; y < CHUNK_SIZE; ++y)
			{
				const glm::ivec2 pos = {x,y};
				chunk.setLightLevel(pos,lightMap[x * CHUNK_SIZE + y]);
			}
		}
		handler.save(chunk);
	}
	else
	{
		for (auto x = 0; x < CHUNK_SIZE; ++x)
		{
			for (auto y = 0; y < CHUNK_SIZE; ++y)
			{
				const glm::ivec2 pos = {x,y};
				chunks[a_pos.x][a_pos.y].setLightLevel(pos,lightMap[x * CHUNK_SIZE + y]);
			}
		}
	}
}

inline void World::GetChunk(const int x, const int y, const glm::ivec2 current)
{
	glm::ivec2 pos(x, y);
	ArrayToChunk(pos,current);
	Chunk& chunk = chunks[x][y];
	chunk.position = pos;
	handler.fetch(chunk);
}

void World::post_generation(Chunk& chunk)
{
	static Util::EnumProbabilityGroup<Probability,3> surface({
		Probability::PLANT,
		Probability::TREE,
		Probability::NONE,
	});

	for (int x = 0; x < CHUNK_SIZE; ++x)
	{
		glm::ivec2 pos(x,0);
		ChunkToGlobal(pos,chunk.position);
		pos.y = biome->getSurface(pos.x) + 1;
		if (chunk.contains(pos))
		{
			// generated plants will be direction dependent and seed dependent
			switch (surface.get())
			{
			case Probability::TREE:
				// prevent trees from spawning next to each other
				if (pos.x % 2 == 0) break;
				// loop through tree structure and apply offsets
				for (Block& block : biome->getTree())
				{
					block.position += pos;
					Block replaced = getBlock(block.position);
					if (replaced.isEmpty())
					{
						setBlock(block);
					}
					else if (replaced.isTransparent() && replaced.getWall() == EMPTY)
					{
						const BlockType wall = block.getWall();
						if (wall != EMPTY)
						{
							replaced.setWall(wall);
							setBlock(replaced);
						}
					}
				}
				break;
			case Probability::PLANT:
				{
					Block plant = biome->getPlant();
					plant.position += pos;
					setBlock(plant);
				}
				break;
			case Probability::NONE:
				break;
			}
		}
	}

	Generate::BlockBorder(this,chunk.position);

	chunk.flag |= POST_GENERATED;
}

inline void World::update_chunks()
{
	glm::ivec2 current = ToChunkSpace(origin);

	// detected chunk origin change
	if (current != chunk_origin){

		update = true;

		auto direction = current - chunk_origin;
		auto quantity = glm::abs(direction);

		int count = 0;
		// update chunks
		for (auto y = 0; y < WORLD_HEIGHT; ++y)
		{
			for (auto x = 0; x < WORLD_WIDTH; ++x)
			{
				// check if shifting is applicable
				if (quantity.x >= WORLD_WIDTH || quantity.y >= WORLD_HEIGHT)
				{
					// fetch all chunks
					handler.save(chunks[x][y]);
					GetChunk(x,y,current);
					++count;
					continue;
				}
				int x_index = direction.x < 0 ? (WORLD_WIDTH - 1) - x : x;
				int y_index = direction.y < 0 ? (WORLD_HEIGHT - 1) - y : y;

				bool is_trailing_edge_x = (direction.x < 0 && x_index == WORLD_WIDTH - 1) ||
									(direction.x > 0 && x_index == 0);

				bool is_trailing_edge_y = (direction.y < 0 && y_index == WORLD_HEIGHT - 1) ||
										  (direction.y > 0 && y_index == 0);

				// saving
				if (is_trailing_edge_x || is_trailing_edge_y)
				{
					handler.save(chunks[x_index][y_index]);
				}

				// shifting
				int source_x = x_index + direction.x;
				int source_y = y_index + direction.y;

				bool valid_source_x = source_x >= 0 && source_x < WORLD_WIDTH;
				bool valid_source_y = source_y >= 0 && source_y < WORLD_HEIGHT;

				if (valid_source_x && valid_source_y)
				{
					chunks[x_index][y_index] = chunks[source_x][source_y];
				}
				else
				{
					// replace old chunk
					GetChunk(x_index,y_index,current);
					++count;
				}
			}
		}
		std::cout << "Displacement in chunks: " << direction.x << " " << direction.y;
		std::cout << "\nChunks fetched from region: " << count << std::endl;
		chunk_origin = current;

		for (auto y = POST_GEN_BUF; y < WORLD_HEIGHT - POST_GEN_BUF; ++y)
		{
			for (auto x = POST_GEN_BUF; x < WORLD_WIDTH - POST_GEN_BUF; ++x)
			{
				// do post generation
				if (!(chunks[x][y].flag & POST_GENERATED))
				{
					post_generation(chunks[x][y]);
				}
				if (!chunks[x][y].lightUpdated)
				{
					Generate::Lighting(this,chunks[x][y].position);
					chunks[x][y].lightUpdated = true;
				}
			}
		}
	}
}