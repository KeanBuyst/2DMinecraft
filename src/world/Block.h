#pragma once

#include <vector>
#include <glm.hpp>

#include "../Constants.h"
#include "../resources/Metadata.h"

namespace world 
{
	enum BlockType : uint8_t
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
		REDSTONE_ORE,
		TORCH,
		STONE_WALL
	};
	// Contains a snapshot of chunk info at a position at the time the getBlock function was called
	class Block
	{
	public:
		Block(BlockType type,glm::vec2 position,BlockType wall);
		Block(glm::vec2 position,uint32_t data);

		glm::vec2 position;

		[[nodiscard]] bool isTransparent() const;
		[[nodiscard]] bool isCollidable() const;
		[[nodiscard]] bool isEmpty() const;
		[[nodiscard]] BlockType getType() const;
		[[nodiscard]] BlockType getWall() const;
		[[nodiscard]] int getLightLevel() const;

		[[nodiscard]] short getInterference() const;

		void setWall(BlockType wall);
		void setType(BlockType type);

		[[nodiscard]] uint32_t getRaw() const;
	protected:
		uint32_t data;
	};
}

