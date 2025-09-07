#include "World.h"

#include <iostream>

#include "../Util.h"
#include "vector"
#include "../Application.h"
#include "generation/Generation.h"

using namespace world;

RegionHandler world::handler = RegionHandler();
World world::m_world;

struct BlockData
{
	float x,y;
	uint32_t data;
};

void World::init()
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
	for (auto wx = 0; wx < WORLD_WIDTH; wx++)
	{
		for (auto wy = 0; wy < WORLD_HEIGHT; wy++)
		{
			Chunk& chunk = chunks[wx][wy];
			if (!(chunk.flag & POST_GENERATED))
			{
				post_generation(chunk);
			}
			Generate::Lighting(this,chunk.position);
		}
	}

	// set up gpu for world render
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Position attribute (x,y)
	glVertexAttribPointer(0, 2,GL_FLOAT, GL_FALSE, sizeof(BlockData), nullptr);
	glEnableVertexAttribArray(0);
	// data input
	glVertexAttribIPointer(1, 1,GL_UNSIGNED_INT,sizeof(BlockData), reinterpret_cast<void *>(offsetof(BlockData,data)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	delete biome;
}

void World::render()
{
	// compile chunks
	constexpr int size = WORLD_WIDTH * WORLD_HEIGHT * CHUNK_SIZE * CHUNK_SIZE;
	std::vector<BlockData> vertices;
	vertices.reserve(size);
	// load chunks for new position (if new position)
	update_chunks();
	
	for (auto chunk_y = 0; chunk_y < WORLD_HEIGHT; chunk_y++)
	{
		for (auto chunk_x = 0; chunk_x < WORLD_WIDTH; chunk_x++)
		{
			Chunk& chunk = chunks[chunk_x][chunk_y];
			for (auto x = 0; x < CHUNK_SIZE; ++x)
			{
				for (auto y = 0; y < CHUNK_SIZE; ++y)
				{
					Block block = chunk.getBlock(glm::ivec2(x,y));
					if (block.isEmpty()) continue;
					glm::vec2 pixelCoord = block.position;
					pixelCoord -= origin;
					pixelCoord *= PIXEL_SCALE; // size each texture in the scene is 16x16 (atlas 8x8). This helps prevent atrificing if it were kept at 1x1.

					// optimizations
					if (pixelCoord.x + PIXEL_SCALE < VIEW_PORT.x || pixelCoord.x > VIEW_PORT.y ||
						pixelCoord.y + CHUNK_SIZE*PIXEL_SCALE < VIEW_PORT.z || pixelCoord.y - CHUNK_SIZE*PIXEL_SCALE > VIEW_PORT.w)
					{
						continue;
					}

					vertices.push_back({
						pixelCoord.x,
						pixelCoord.y,
						block.getRaw()
					});
				}
			}
		}
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(BlockData), vertices.data(), GL_STATIC_DRAW);

	glDrawArrays(GL_POINTS, 0, vertices.size());
}

void World::setBlock(const Block& block, const bool doPostUpdate)
{
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
		auto direction = current - chunk_origin;
		auto quantity = glm::abs(direction);
		// save all chunks & post generation
		for (auto y = 0; y < WORLD_HEIGHT; ++y)
			for (auto x = 0; x < WORLD_WIDTH; ++x)
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
				// save all chunks
				handler.save(chunks[x][y]);
			}
		// update chunks
		for (auto y = 0; y < WORLD_HEIGHT; ++y)
		{
			for (auto x = 0; x < WORLD_WIDTH; ++x)
			{
				// check if shifting is applicable
				if (quantity.x >= WORLD_WIDTH || quantity.y >= WORLD_HEIGHT)
				{
					// shifting not applicable
					glm::ivec2 pos(x, y);
					ArrayToChunk(pos,current);
					Chunk& chunk = chunks[x][y];
					handler.save(chunk);
					chunk.position = pos;
					handler.fetch(chunk);
				}
				else
				{
					int index;
					// horizontal shifting
					if (quantity.x != 0) // don't generate if no change accorded
					{
						index = direction.x < 0 ? (WORLD_WIDTH - 1) - x : x;
						if (x <= quantity.x)
						{
							const Chunk& chunk = chunks[index + direction.x][y];
							chunks[index][y] = chunk;
						}
						else
						{
							// replace old chunk
							GetChunk(index, y,current);
							continue;
						}
					}
					// vertical shifting
					if (quantity.y != 0)
					{
						index = direction.y < 0 ? (WORLD_HEIGHT - 1) - y : y;
						if (y <= quantity.y)
						{
							chunks[x][index] = chunks[x][index + direction.y];
						}
						else if (quantity.y != 0)
						{
							// replace old chunk
							GetChunk(x, index,current);
							continue;
						}
					}
				}
			}
		}
		chunk_origin = current;
	}
}