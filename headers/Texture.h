#pragma once

#include <glew.h>

namespace res
{
	class Texture 
	{
	public:
		explicit Texture(const char* path);
		~Texture();

		void bind(unsigned int slot) const;

		void unbind();

		GLuint ID;
	};
}

