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

template<typename T>
static void decompress(std::ifstream& stream, T* ptr, size_t size)
{
	const T* end = ptr + size;
	while (ptr < end)
	{
		char byteFlags;
		if (!stream.read(&byteFlags, sizeof(byteFlags))) return;

		for (size_t payload_index = 0; payload_index < 8 && ptr < end; ++payload_index)
		{
			if (byteFlags & (1 << payload_index))
			{
				uint16_t runLength;
				if (!stream.read(reinterpret_cast<char*>(&runLength), sizeof(runLength)))
					return;

				T type;
				if (!stream.read(reinterpret_cast<char*>(&type), sizeof(T)))
					return;

				for (uint32_t i = 0; i <= runLength && ptr < end; ++i)
				{
					*(ptr++) = type;
				}
			}
			else
			{
				if (!stream.read(reinterpret_cast<char*>(ptr++), sizeof(T)))
					return;
			}
		}
	}
}

Region::Region(const glm::ivec2 position) : position(position), fileName(basePath + name(position) + ".region")
{
	std::ifstream file(fileName,std::ios::binary | std::ios::in);

	if (file.is_open())
	{
		// decompression
		decompress(file,flags, SIZE);
		decompress(file,tileBuffer,BUFFER_SIZE);

		// load entity region data
		uint16_t size;
		int chunkIndex = 0;
		while (file.peek() != EOF)
		{
			file.read(reinterpret_cast<char*>(&size),2);
			if (size != 0)
			{
				entityChunkBuffer[chunkIndex].resize(size);
				file.read(reinterpret_cast<char*>(entityChunkBuffer[chunkIndex].data()),size);
			}
			++chunkIndex;
		}

		std::cout << "Loading region at " << position.x << "," << position.y << std::endl;
	}
	else std::cout << "Creating region at " << position.x << "," << position.y << std::endl;
}

template<typename T>
static void compress(std::ofstream& stream,const T* ptr, size_t size)
{
	char byteFlags;
	// byte group
	uint16_t runLength;
	T type;

	static constexpr size_t payload_capacity = 1 + ((sizeof(runLength) + sizeof(type)) * (sizeof(byteFlags) * 8));
	char payload[payload_capacity];

	const T* end = ptr + size;
	while (ptr < end)
	{
		size_t payload_index = 1;
		byteFlags = 0;
		for (size_t byte_i = 0; byte_i < 8; ++byte_i)
		{
			if (ptr == end) break;
			type = *(ptr++);
			if (ptr < end && std::memcmp(ptr, &type, sizeof(T)) == 0)
			{
				runLength = 0;
				byteFlags |= 1 << byte_i;
				while (ptr < end && std::memcmp(ptr, &type, sizeof(T)) == 0 &&
					runLength < std::numeric_limits<uint16_t>::max())
				{
					++runLength;
					++ptr;
				}

				std::memcpy(payload + payload_index, &runLength, sizeof(runLength));
				payload_index += sizeof(runLength);
			}
			std::memcpy(payload + payload_index, &type, sizeof(T));
			payload_index += sizeof(T);
		}
		payload[0] = byteFlags;
		stream.write(payload,payload_index);
	}
}

Region::~Region()
{
	// write tiles to data file

	std::ofstream file(fileName,std::ios::binary | std::ios::out);

	compress(file,flags, SIZE);
	compress(file,tileBuffer, BUFFER_SIZE);

	// write entities to data file
	for (auto index = 0; index < SIZE; ++index)
	{
		auto& array = entityChunkBuffer[index];

		uint16_t size = static_cast<uint16_t>(array.size());
		file.write(reinterpret_cast<char*>(&size),2);

		if (array.empty()) continue;

		file.write(reinterpret_cast<char*>(array.data()),size);
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
	std::cout << GetIndex(chunk_entities) << std::endl;
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
	int x,y;
	if (chunk.x < 0) x = (REGION_SIZE - 1) + (chunk.x % REGION_SIZE);
	else x = chunk.x % REGION_SIZE;
	if (chunk.y < 0) y = (REGION_SIZE - 1) + (chunk.y % REGION_SIZE);
	else y = chunk.y % REGION_SIZE;

	return x + y * REGION_SIZE; // account for rows
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
