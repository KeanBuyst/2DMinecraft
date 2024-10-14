#pragma once

#include <glm.hpp>
#include "Metadata.h"

namespace world 
{
	enum MATERIAL : unsigned char
	{
		AIR,
		GRASS_BLOCK,
		DIRT,
		OAK_LOG,
		STONE,
		GRASS,
		OAK_LEAVES
	};

	class Block
	{
	public:
		Block(MATERIAL type,glm::ivec2 postion);

		MATERIAL type;
		glm::ivec2 pos;
	};

	class MetaBlock : Block
	{
	public:
		MetaBlock(MATERIAL type, glm::ivec2 postion);
		void addMeta(Metadata data);
	};
}

