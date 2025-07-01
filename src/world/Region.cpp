#include "Region.h"

#include <iostream>
#include <string>
#include <cstring>

#include "../resources/Storage.h"
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
		file.read(buffer,BUFFER_SIZE);

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
	file.write(buffer, BUFFER_SIZE);

	std::cout << "Unloading region at " << position.x << "," << position.y << std::endl;
}

void Region::fetch(Chunk &chunk)
{
	// local position in region
	
	const int index = GetIndex(chunk);
	uint8_t& flag = flags[index];

	if (flag & FLAGS::GENERATED)
	{
		memcpy(chunk.blocks, buffer + index * CHUNK_SIZE*CHUNK_SIZE, sizeof(uint32_t) * CHUNK_SIZE*CHUNK_SIZE);
	}
	else 
	{
		chunk.generate();
		// but generated data into buffer
		memcpy(buffer + index * CHUNK_SIZE*CHUNK_SIZE, chunk.blocks, sizeof(uint32_t) * CHUNK_SIZE*CHUNK_SIZE);
		flag = GENERATED;
	}

	chunk.flag = flag;
}

bool Region::contains(const glm::ivec2 chunk) const
{
	return ToRegionSpace(chunk) == position;
}

void Region::save(const Chunk& chunk)
{
	const int index = GetIndex(chunk);
	flags[index] = chunk.flag;
	memcpy(buffer + index * CHUNK_SIZE*CHUNK_SIZE, chunk.blocks, sizeof(uint32_t) * CHUNK_SIZE*CHUNK_SIZE);
}

inline int world::Region::GetIndex(const Chunk& chunk) {
	const glm::ivec2 pos = glm::abs(chunk.position % REGION_SIZE);
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
