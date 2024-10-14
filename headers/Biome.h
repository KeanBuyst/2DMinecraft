#pragma once

#include "Block.h"

namespace world
{
	class Biome
	{
	public:
		Biome(float smoothness, int min_max);
		MATERIAL getMaterial(int layer);

		float smoothness;
		int min_max;
	};

	extern Biome* biome;
}