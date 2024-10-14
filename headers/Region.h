#pragma once

#include "Chunk.h"

namespace world 
{
	constexpr int REGION_SIZE = 32;
	constexpr int STACK_SIZE = 4;

	enum FLAGS : uint8_t
	{
		GENERATED = 0b1
	};

	class Region
	{
	public:
		glm::ivec2 position;

		explicit Region(glm::ivec2 position);
		~Region();

		void fetch(Chunk& chunk) const;
		bool contains(glm::ivec2 chunk) const;

		void save(const Chunk& chunk);
	private:
		uint8_t flags[REGION_SIZE * REGION_SIZE] = { 0 };
		uint64_t buffer[REGION_SIZE * REGION_SIZE * CHUNK_SIZE] = { 0 };

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
		uint8_t call_miss[STACK_SIZE] = { 0 };
		uint8_t size = 0;
		Region* stack[STACK_SIZE] = { nullptr };

		Region* GetRegion(glm::ivec2 pos);
	};
}

