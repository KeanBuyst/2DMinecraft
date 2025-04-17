#pragma once

#include "Block.h"

namespace world
{
	extern const Block OAK_TREE[];

	enum class Probability : short
	{
		TREE = 40,
		PLANT = 60,
		NONE = 50,
	};

	class Biome
	{
	public:
		virtual ~Biome() = default;

		virtual MATERIAL getMaterial(int depth);

		virtual Block getPlant();
		virtual std::vector<Block> getTree() = 0;
		virtual float getNoiseScale() = 0;
		virtual float getAmplifier() = 0;
		virtual int getSurface(float x);
	};

	class Forest final : public Biome
	{
	public:
		std::vector<Block> getTree() override;
		float getNoiseScale() override;
		float getAmplifier() override;
	};

	extern Biome* biome;
}