#pragma once

#include <vector>
#include <glm.hpp>

#include "../Constants.h"
#include "../resources/Metadata.h"

namespace world 
{
	typedef bool Layer;

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
		Block(MATERIAL type,glm::ivec2 position,Layer layer,uint8_t luminance);

		const MATERIAL type;
		const glm::ivec2 position;
		const Layer layer;
		const uint8_t luminance; // from 0 to 15

		bool isTransparent(); // TODO make this depicted by material type
	};
}

