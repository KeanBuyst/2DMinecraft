#include "Region.h"

#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <string_view>

#include "../resources/Storage.h"
#include "entities/EntityHandler.h"
#include "generation/Generation.h"

using namespace world;
using namespace res;

constexpr int SIZE = REGION_SIZE * REGION_SIZE;
constexpr int BUFFER_SIZE = SIZE * CHUNK_SIZE * CHUNK_SIZE;
constexpr uint8_t attenuation = 10;

std::string name(const glm::ivec2& position)
{
	return std::to_string(position.x) + "." + std::to_string(position.y);
}

Region::Region(const glm::ivec2 position) : position(position)
{
	DataFile file(name(position) + ".region",std::ios::in);
	if (file.available())
	{
		file.read(flags, SIZE);
		file.read(tileBuffer,BUFFER_SIZE);

		// load entity region data
		uint16_t size;
		int chunkIndex = 0;
		while (file.hasNext())
		{
			file.read(&size,1);
			if (size == 0) continue;
			std::cout << "NUM BYTES OF ENTITIES: " << size << " chunk index " << chunkIndex << std::endl;

			entityChunkBuffer[chunkIndex].resize(size);
			file.read(entityChunkBuffer[chunkIndex].data(),size);

			++chunkIndex;
		}

		std::cout << "Loading region at " << position.x << "," << position.y << std::endl;
	}
	else std::cout << "Creating region at " << position.x << "," << position.y << std::endl;
}

Region::~Region()
{
	/*
	 Error accorded here when setting block (block outside loaded chunks)
	 and loading region at same time. At least I believe this was the cause. Accorded during
	 logic error of setting plant. Important notice for potential similar future errors.
	 */
	DataFile file(name(position) + ".region",std::ios::out);

	file.write(flags, SIZE);
	file.write(tileBuffer, BUFFER_SIZE);

	for (auto index = 0; index < SIZE; ++index)
	{
		auto& array = entityChunkBuffer[index];

		uint16_t size = static_cast<uint16_t>(array.size());
		file.write(&size,1);

		if (array.empty()) continue;

		file.write(array.data(),size);
	}

	file.flush();

	std::cout << "Unloading region at " << position.x << "," << position.y << std::endl;
}

void Region::fetch(Chunk &chunk)
{
	// local position in region
	const int index = GetIndex(chunk.position);
	uint8_t& flag = flags[index];

	if (flag & FLAGS::GENERATED)
	{
		memcpy(chunk.blocks, tileBuffer + index * CHUNK_SIZE*CHUNK_SIZE, sizeof(uint32_t) * CHUNK_SIZE*CHUNK_SIZE);
	}
	else 
	{
		chunk.generate();
		// but generated data into buffer
		memcpy(tileBuffer + index * CHUNK_SIZE*CHUNK_SIZE, chunk.blocks, sizeof(uint32_t) * CHUNK_SIZE*CHUNK_SIZE);
		flag = GENERATED;
	}

	// load entities from chunk
	std::vector<uint8_t>& array = entityChunkBuffer[index];
	if (!array.empty())
	{
		Util::ByteStream stream(&array);
		// adds all entities
		while (stream.hasNext())
		{
			if (!EntityHandler::Add(stream))
			{
				throw "CORRUPTED DATA: Unable to read entity from region file";
			}
		}
		// remove contents for re-addition during saving
		array.clear();
	}

	chunk.lightUpdated = false;
	chunk.flag = flag;
}

std::vector<uint8_t>& Region::fetch(glm::ivec2 chunk_entities)
{
	return entityChunkBuffer[GetIndex(chunk_entities)];
}

bool Region::contains(const glm::ivec2 chunk) const
{
	return ToRegionSpace(chunk) == position;
}

void Region::save(const Chunk& chunk)
{
	const int index = GetIndex(chunk.position);
	flags[index] = chunk.flag;
	memcpy(tileBuffer + index * CHUNK_SIZE*CHUNK_SIZE, chunk.blocks, sizeof(uint32_t) * CHUNK_SIZE*CHUNK_SIZE);
}

inline int Region::GetIndex(const glm::ivec2& chunk) {
	const glm::ivec2 pos = glm::abs(chunk % REGION_SIZE);
	return pos.x + pos.y * REGION_SIZE; // account for rows
}

RegionHandler::~RegionHandler()
{
	for (auto i = 0; i < size; i++)
		delete stack[i];
}

Region* RegionHandler::GetRegion(const glm::ivec2 pos)
{
	for (auto i = 0; i < size; i++)
	{
		Region*& region = stack[i];
		if (region->contains(pos))
		{
			return region;
		}
	}
	auto* reg = new Region(ToRegionSpace(pos));
	if (size < STACK_SIZE)
	{
		stack[size] = reg;
		size++;
	}
	else
	{
		// no more spaces left in stack. Replace first and oldest element
		delete stack[old];
		stack[old] = reg;
		old++;
		if (old == STACK_SIZE) old = 0;
	}
	return reg;
}

void RegionHandler::fetch(Chunk& chunk)
{
	GetRegion(chunk.position)->fetch(chunk);
}

std::vector<uint8_t>& RegionHandler::fetch(glm::ivec2 chunk_entities)
{
	return GetRegion(chunk_entities)->fetch(chunk_entities);
}

void RegionHandler::save(const Chunk& chunk)
{
	GetRegion(chunk.position)->save(chunk);
}

glm::ivec2 world::ToRegionSpace(const glm::ivec2 pos)
{
	// fix division problem. E.g. -1 / 32 = 0 && 1 / 32 = 0
	glm::ivec2 fix = pos / REGION_SIZE;
	if (pos.x < 0) fix.x--;
	if (pos.y < 0) fix.y--;
	return fix;
}
