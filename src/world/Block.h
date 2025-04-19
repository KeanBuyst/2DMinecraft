#pragma once

#include <vector>
#include <glm.hpp>

#include "../Constants.h"
#include "../resources/Metadata.h"

namespace world 
{
	enum MATERIAL : uint8_t
	{
		EMPTY,
		GRASS_BLOCK,
		DIRT,
		OAK_LOG,
		STONE,
		GRASS,
		OAK_LEAVES,
		IRON_ORE,
		COAL_ORE,
		DIAMOND_ORE,
		GOLD_ORE,
		COPPER_ORE,
		LAPIS_ORE,
		EMERALD_ORE,
		REDSTONE_ORE
	};
	// Contains a snapshot of chunk info at a position at the time the getBlock function was called
	class Block
	{
	public:
		Block(MATERIAL type,glm::vec2 position,MATERIAL wall);
		Block(glm::vec2 position,uint32_t data);

		glm::vec2 position;

		[[nodiscard]] bool isTransparent() const;
		[[nodiscard]] bool isEmpty() const;
		[[nodiscard]] MATERIAL getType() const;
		[[nodiscard]] MATERIAL getWall() const;
		[[nodiscard]] int getLuminance() const;

		void setWall(MATERIAL wall);
		void setType(MATERIAL type);

		[[nodiscard]] uint32_t getRaw() const;
	protected:
		uint32_t data;
	};
}

