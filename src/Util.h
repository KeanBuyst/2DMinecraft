#pragma once
#include <array>
#include <random>
#include <stdexcept>

namespace Util 
{
	extern unsigned int seed_value;
	extern std::mt19937 rng;
	void seed(unsigned int seed);
	void seed();
	
	/*
	* Seed function must be called first
	* @return value between [-1,1]
	*/
	float noise(float x);
	float noise(float x,float y);

	template<typename T>
	struct ProbabilitySet {
		T item;
		float weight;

		ProbabilitySet(T item, const float weight) : item(item), weight(weight) {
			if(weight < 0.0f)
			{
				const std::string message = std::string("Weight(") + std::to_string(weight) + std::string(") cannot be negative");
				throw std::invalid_argument(message);
			}
		}
	};

	template<typename T,size_t size>
	class ProbabilityGroup
	{
	public:
		explicit ProbabilityGroup(std::array<ProbabilitySet<T>,size> sets) : items(sets)
		{
			std::array<float,size> weights;
			for (auto i = 0; i < sets.size(); ++i)
			{
				weights[i] = sets[i].weight;
			}
			dist = std::discrete_distribution<size_t>(weights.begin(), weights.end());
		}

		T get()
		{
			static_assert(size > 0, "Cannot have an empty probability group");
			return items[dist(rng)].item;
		}
	private:
		std::array<ProbabilitySet<T>,size> items;
		std::discrete_distribution<size_t> dist;
	};

	template<typename Enum, size_t size>
	class EnumProbabilityGroup
	{
		static_assert(std::is_enum_v<Enum>, "Template parameter must be an enum type");
	public:
		explicit EnumProbabilityGroup(const std::array<Enum, size>& enumValues)
			: values(enumValues)
		{
			std::array<float,size> weights;
			for (auto i = 0; i < enumValues.size(); ++i)
			{
				weights[i] = static_cast<float>(enumValues[i]);
			}
			dist = std::discrete_distribution<size_t>(weights.begin(), weights.end());
		}

		Enum get() {
			static_assert(size > 0, "Cannot have an empty probability group");
			return values[dist(rng)];
		}

	private:
		std::array<Enum, size> values;
		std::discrete_distribution<size_t> dist;
	};
}

