#pragma once

#include <cstdint>
#include <glm.hpp>
#include <vector>
#include <World.h>

namespace Util 
{

	void seed(unsigned int seed);
	void seed();
	
	/*
	* Seed function must be called first
	* @return value between [-1,1]
	*/
	float noise(float x);
}

