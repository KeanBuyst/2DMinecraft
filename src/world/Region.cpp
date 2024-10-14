#include "Region.h"

#include <iostream>
#include <string>

#include "Storage.h"

using namespace world;
using namespace res;

constexpr int SIZE = REGION_SIZE * REGION_SIZE;
constexpr int BUFFER_SIZE = SIZE * CHUNK_SIZE;

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
		file.read(buffer, BUFFER_SIZE);
	}
	std::cout << "Loading region at " << position.x << "," << position.y << std::endl;
}

Region::~Region()
{
	DataFile file(name(position) + ".region",std::ios::out);
	file.write(flags, SIZE);
	file.write(buffer, BUFFER_SIZE);
	std::cout << "Unloading region at " << position.x << "," << position.y << std::endl;
}

void Region::fetch(Chunk &chunk) const
{
	// local position in region
	
	const int index = GetIndex(chunk);
	const uint8_t flag = flags[index];

	if (flag & FLAGS::GENERATED)
	{
		memcpy(chunk.blocks, buffer + index * CHUNK_SIZE, sizeof(uint64_t) * CHUNK_SIZE);
	}
	else 
	{
		chunk.generate();
	}
}

bool Region::contains(glm::ivec2 pos) const
{
	return ToRegionSpace(pos) == position;
}

void Region::save(const Chunk& chunk)
{
	const int index = GetIndex(chunk);
	flags[index] = GENERATED;

	memcpy(buffer + index * CHUNK_SIZE, chunk.blocks, sizeof(uint64_t) * CHUNK_SIZE);
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

Region* RegionHandler::GetRegion(glm::ivec2 pos)
{
	//to sort array to remove open spaces
	Region* reg = nullptr;

	for (auto i = 0; i < size; i++)
	{
		Region*& region = stack[i];
		assert(region);
		uint8_t& miss = call_miss[i];
		if (region->contains(pos))
		{
			miss = 0;
			reg = region;
		}
		else
		{
			miss++;
			if (miss > 32) {
				delete region;
				region = nullptr;
				size--;

				for (auto s = i; s < size; s++)
				{
					Region*& next = stack[s+1];
					if (next == nullptr) break;
					stack[s] = next;
					next = nullptr;
				}

				if (region != nullptr) i--;
			}
		}
	}
	if (reg == nullptr)
	{
		// load new region
		reg = new Region(ToRegionSpace(pos));
		if (size < STACK_SIZE)
		{
			stack[size] = reg;
			call_miss[size] = 0;
			size++;
		}
		else
		{
			// no more spaces left in stack. Replace first and oldest element
			delete stack[0];
			call_miss[0] = 0;
			stack[0] = reg;
		}
	}
	return reg;
}

void RegionHandler::fetch(Chunk& chunk)
{
	GetRegion(chunk.position)->fetch(chunk);
	/*if (!stack[0])
		stack[0] = new Region(ToRegionSpace(chunk.position));
	if (stack[0]->contains(chunk.position)) {
		stack[0]->fetch(chunk);
	} else {
		chunk = Chunk();
		printf("Requested (fetch) chunk out of bounds: %i, %i\n",chunk.position.x,chunk.position.y);
	}*/
}

void RegionHandler::save(const Chunk& chunk)
{
	GetRegion(chunk.position)->save(chunk);
	/*if (!stack[0])
		stack[0] = new Region(ToRegionSpace(chunk.position));
	if (stack[0]->contains(chunk.position)) {
		stack[0]->save(chunk);
	} else {
		printf("Requested (save) chunk out of bounds: %i, %i\n",chunk.position.x,chunk.position.y);
	}*/
}

glm::ivec2 world::ToRegionSpace(glm::ivec2 pos)
{
	// fix division problem. E.g. -1 / 32 = 0 && 1 / 32 = 0
	glm::ivec2 fix = pos / REGION_SIZE;
	if (pos.x < 0) fix.x--;
	if (pos.y < 0) fix.y--;
	return fix;
}
