#pragma once

#include <vector>
#include <glm.hpp>

#include "../Constants.h"
#include "../resources/Metadata.h"

namespace world 
{
	enum MATERIAL : unsigned char
	{
		EMPTY,
		GRASS_BLOCK,
		DIRT,
		OAK_LOG,
		STONE,
		GRASS,
		OAK_LEAVES
	};
	// Contains a snapshot of chunk info at a position at the time the getBlock function was called
	class Block
	{
	public:
		Block(MATERIAL type,glm::vec2 position,MATERIAL wall,int luminance = 0);

		const MATERIAL type;
		const glm::vec2 position;
		int luminance; // from 0 to 15

		bool isTransparent() const;
		bool isEmpty() const;
		MATERIAL getWall() const;
	protected:
		const MATERIAL wall;
	};
}

