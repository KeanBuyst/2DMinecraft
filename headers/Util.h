#pragma once

#include <cstdint>

namespace Util 
{
	void seed(unsigned int seed);
	void seed();
	
	/*
	* Seed function must be called first
	* @return value btween [-1,1]
	*/
	float noise(float x);
}

