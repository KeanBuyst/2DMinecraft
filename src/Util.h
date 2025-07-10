#pragma once
#include <array>
#include <cassert>
#include <random>
#include <stdexcept>
#include <glm.hpp>

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

	glm::vec2 rotate(const glm::vec2& point, float angle);

	void decreaseMagnitude(glm::vec2& vector,float scaler);
	void decreaseMagnitude(float& vector,float scaler);

	void drawDebugLines(const glm::vec2* points, int size);

	template<typename T>
	constexpr T DegToRad(T degrees) {
			return degrees * static_cast<T>(M_PI / 180.0);
	}

	template<typename T>
	void clamp(glm::vec<2,T>& vector, T max)
	{
		if (vector.x > max) vector.x = max;
		if (vector.x < -max) vector.x = -max;
		if (vector.y > max) vector.y = max;
		if (vector.y < -max) vector.y = -max;
	}
	template<typename T>
	void clamp(T& vector, T max)
	{
		if (vector > max) vector = max;
		if (vector < -max) vector = -max;
	}

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

	template<typename T, uint16_t max_size>
	struct Buffer
	{
		int size = 0;

		Buffer()
		{
			for (auto i = 0; i < max_size; ++i)
			{
				data[i] = nullptr;
				space[i] = 0;
			}
		}

		~Buffer()
		{
			clean();
		}

		void clean()
		{
			for (auto i = 0; i < max_size; ++i)
			{
				if (data[i] != nullptr)
				{
					delete data[i];
					data[i] = nullptr;
				}
			}
		}

		T* get(uint16_t id)
		{
			return data[id];
		}

		bool next(T*& element)
		{
			if (index == size)
			{
				index = 0;
				return false;
			}
			while (data[index] == nullptr)
			{
				index++;
				if (index == size)
				{
					index = 0;
					return false;
				}
			}
			element = data[index];
			index++;
			return true;
		}

		void destroy(uint16_t id)
		{
			delete data[id];
			data[id] = nullptr;
			space[space_size] = id;
			space_size++;
		}
		// returns ID
		uint16_t add(T* element)
		{
			uint16_t id;
			if (space_size == 0)
			{
				assert(size != max_size);
				data[size] = element;
				id = size;
				size++;
			} else
			{
				space_size--;
				id = space[space_size];
				data[id] = element;
			}
			return id;
		}
	private:
		T* data[max_size];
		uint16_t space[max_size];
		int space_size = 0;
		int index = 0;
	};
}

