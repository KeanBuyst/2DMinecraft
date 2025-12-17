#pragma once
#include <cstdint>

#include <glm.hpp>

#include "Frame.h"

namespace gl
{
	class Texture 
	{
	private:
		uint32_t id;
		int width, height;
	public:
		explicit Texture(const char* path);
		~Texture();

		void bind(unsigned int slot) const;
		void unbind();

		uint32_t getID();
		int getWidth();
		int getHeight();

		TextureMap format(float pX, float pY, float pWidth, float pHeight);
	};
}

