#include "World.h"

#include <iostream>

#include "../Util.h"
#include "vector"
#include "../Application.h"

using namespace world;

struct VertexData {
	float x, y;      // The base position (x, y)
	uint64_t blocks; // 64-bit integer representing 8 blocks
	uint8_t depth;
	uint32_t lightMap;
};

glm::vec2 world::origin = glm::vec2(CHUNK_SIZE / 2.0f,CHUNK_SIZE / 2.0f);
RegionHandler world::handler = RegionHandler();

void World::init()
{
	// set up chunks for loading/generation
	Util::seed(WORLD_SEED);
	world::biome = new Biome(0.2f,5);
	chunkOrigin = ToChunkSpace(origin);

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

	// set up gpu for world render
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Position attribute (x,y)
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), nullptr);
	glEnableVertexAttribArray(0);
	// The row for 64 bit int
	glVertexAttribLPointer(1, 1, GL_UNSIGNED_INT64_ARB, sizeof(VertexData), reinterpret_cast<void *>(offsetof(VertexData, blocks)));
	glEnableVertexAttribArray(1);
	// The depth data
	glVertexAttribIPointer(2,1,GL_UNSIGNED_BYTE,sizeof(VertexData),reinterpret_cast<void *>(offsetof(VertexData, depth)));
	glEnableVertexAttribArray(2);
	// light map
	glVertexAttribIPointer(3,1,GL_UNSIGNED_INT,sizeof(VertexData),reinterpret_cast<void *>(offsetof(VertexData, lightMap)));
	glEnableVertexAttribArray(3);

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
	constexpr int size = WORLD_WIDTH * WORLD_HEIGHT * CHUNK_SIZE;
	std::vector<VertexData> vertices(size);
	// load chunks for new position (if new position)
	update_chunks();
	
	for (auto y = 0; y < WORLD_HEIGHT; y++)
	{
		for (auto x = 0; x < WORLD_WIDTH; x++)
		{
			Chunk& chunk = chunks[x][y];
			for (auto colomn = 0; colomn < CHUNK_SIZE; colomn++)
			{
				uint64_t blocks = chunk.blocks[colomn];
				if (blocks == 0) continue;
				glm::ivec2 pos(colomn, 0);
				ChunkToGlobal(pos, chunk.position);
				glm::vec2 pixelCoord(pos);
				pixelCoord -= origin;
				pixelCoord *= PIXEL_SIZE; // size each texture in the scene is 16x16 (atlas 8x8). This helps prevent atrificing if it were kept at 1x1.

				// optimizations
				if (pixelCoord.x + PIXEL_SIZE < VIEW_PORT.x || pixelCoord.x > VIEW_PORT.y ||
					pixelCoord.y + CHUNK_SIZE*PIXEL_SIZE < VIEW_PORT.z || pixelCoord.y - CHUNK_SIZE*PIXEL_SIZE > VIEW_PORT.w)
				{
					continue;
				}

				VertexData data = {
					pixelCoord.x,
					pixelCoord.y,
					blocks,
					chunk.depth[colomn],
					chunk.lightMap[colomn],
				};

				vertices.push_back(data);
			}
		}
	}
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), vertices.data(), GL_STATIC_DRAW);

	glDrawArrays(GL_POINTS, 0, vertices.size());
}

void World::setBlock(const Block block,const bool force)
{
	glm::ivec2 c_pos;
	// floor brings -0.5 to -1 and 0.5 to 0
	glm::ivec2 pos(floorf(block.position.x),floorf(block.position.y));
	GlobalToChunk(pos, c_pos);
	if(glm::ivec2 a_pos = c_pos; !ChunkToArray(a_pos))
	{
		if (force)
		{
			Chunk chunk;
			chunk.position = c_pos;
			handler.fetch(chunk);
			chunk.setBlock(pos, block);
			handler.save(chunk);
		}
		std::cerr << "Attempted to alter unloaded block (force is false)\nIgnoring request" << std::endl;
	}
	else
	{
		chunks[a_pos.x][a_pos.y].setBlock(pos,block);
	}
}

Block World::getBlock(const glm::vec2 position, const bool force) const
{
	// position relative to origin due to loaded chunks being around origin
	glm::ivec2 c_pos;
	glm::ivec2 pos(floorf(position.x),floorf(position.y));
	GlobalToChunk(pos, c_pos);
	if(glm::ivec2 a_pos = c_pos; !ChunkToArray(a_pos))
	{
		if (force)
		{
			Chunk chunk;
			chunk.position = c_pos;
			handler.fetch(chunk);
			return chunk.getBlock(pos);
		}
		std::cerr << "Attempted to access unloaded block (force is false)\nZReturning EMPTY block" << std::endl;
		return {
			EMPTY,
			position,
			0,0,
		};
	}
	else
	{
		return chunks[a_pos.x][a_pos.y].getBlock(pos);
	}
}

inline void World::generate(const int x, const int y)
{
	glm::ivec2 pos(x, y);
	ArrayToChunk(pos);
	Chunk& chunk = chunks[x][y];
	handler.save(chunk);
	chunk.position = pos;
	handler.fetch(chunk);
}
inline void World::update_chunks()
{
	glm::ivec2 current = ToChunkSpace(origin);
	
	if (current != chunkOrigin)
	{
		auto direction = current - chunkOrigin;
		auto quantity = glm::abs(direction);

		for (auto x = 0; x < WORLD_WIDTH; x++)
		{
			for (auto y = 0; y < WORLD_HEIGHT; y++)
			{
				// check if shifting is applicable
				if (quantity.x >= WORLD_WIDTH || quantity.y >= WORLD_HEIGHT)
				{
					generate(x,y);
				}
				else
				{
					int index;
					if (quantity.x != 0) // don't generate if no change accorded
					{
						index = direction.x < 0 ? (WORLD_WIDTH - 1) - x : x;
						if (x <= quantity.x)
						{
							chunks[index][y] = chunks[index + direction.x][y];
						}
						else
						{
							generate(index, y);
							continue;
						}
					}
					if (quantity.y != 0)
					{
						index = direction.y < 0 ? (WORLD_HEIGHT - 1) - y : y;
						if (y <= quantity.y)
						{
							chunks[x][index] = chunks[x][index + direction.y];
						}
						else if (quantity.y != 0)
						{
							generate(x, index);
						}
					}
				};
			}
		}
		chunkOrigin = current;
	}
}
inline glm::ivec2 World::ToChunkSpace(const glm::vec2 pos)
{
	// fix division problem. E.g. -1 / 32 = 0 && 1 / 32 = 0
	const glm::vec2 fix = glm::floor(pos / static_cast<float>(CHUNK_SIZE));
	return glm::ivec2(fix);
}
// Bool determins if its a successful choord in the chunk or its not within the loaded chunks
inline bool World::ChunkToArray(glm::ivec2& chunk)
{
	const glm::ivec2 centre = ToChunkSpace(origin);
	chunk += centre;
	chunk.x += WORLD_WIDTH / 2;
	chunk.y += WORLD_HEIGHT / 2 + 1;
	return chunk.x >= 0 && chunk.x < WORLD_WIDTH && chunk.y >= 0 && chunk.y < WORLD_HEIGHT;
}
inline void World::ArrayToChunk(glm::ivec2& chunk)
{
	const glm::ivec2 centre = ToChunkSpace(origin);
	chunk.x -= WORLD_WIDTH / 2;
	chunk.y -= WORLD_HEIGHT / 2 - 1;
	chunk += centre;
}
// Position to chunk local position and chunk position
inline void World::GlobalToChunk(glm::ivec2& position,glm::ivec2& chunk)
{
	chunk = ToChunkSpace(position);
	position = glm::abs(position % CHUNK_SIZE);
}
// Position in chunk to global position
inline void World::ChunkToGlobal(glm::ivec2& position,glm::ivec2 chunk)
{
	if (chunk.x < 0)
	{
		chunk.x++;
		position.x = -(position.x + 1);
	}
	if (chunk.y < 0)
	{
		chunk.y++;
		position.y = -(position.y + CHUNK_SIZE);
	}
	position += chunk * CHUNK_SIZE;
}