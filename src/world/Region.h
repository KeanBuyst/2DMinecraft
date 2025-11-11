#pragma once

#include "Chunk.h"
#include "entities/Entity.h"

namespace world 
{
	extern short DAYLIGHT;

	enum FLAGS : uint8_t
	{
		GENERATED = 0b1,
		POST_GENERATED = 0b10
	};

	class Region
	{
	public:
		glm::ivec2 position;

		explicit Region(glm::ivec2 position);
		~Region();

		void fetch(Chunk& chunk);
		bool contains(glm::ivec2 chunk) const;

		void save(const Chunk& chunk);
	private:
		uint8_t flags[REGION_SIZE * REGION_SIZE] = { 0 };
		uint32_t tileBuffer[REGION_SIZE * REGION_SIZE * CHUNK_SIZE * CHUNK_SIZE] = { 0 };

		static inline int GetIndex(const Chunk& chunk);
	};
	
	glm::ivec2 ToRegionSpace(glm::ivec2 pos);

	class RegionHandler 
	{
	public:
		~RegionHandler();

		void fetch(Chunk& chunk);
		void save(const Chunk &chunk);
	private:
		// max 4 regions for times of intersection between 4 different regions
		uint8_t size = 0;
		uint32_t old = 0;
		Region* stack[STACK_SIZE] = { nullptr };

		Region* GetRegion(glm::ivec2 pos);
	};
}

