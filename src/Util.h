#pragma once
#include <array>
#include <stdexcept>

namespace Util 
{

	void seed(unsigned int seed);
	void seed();
	
	/*
	* Seed function must be called first
	* @return value between [-1,1]
	*/
	float noise(float x);
	float noise(float x,float y);
}

