#include "Texture.h"
#include "../libs/stb_image.h"
#include <iostream>
#include "../glew.h"

using namespace gl;

Texture::Texture(const char* path)
{
	int m_BPP;
	
	void* pixels = stbi_load(path, &width, &height, &m_BPP, 4);

	if (!pixels)
	{
		std::cerr << "No image found at path: " << path <<std::endl;
		exit(-1);
	}

	// create openGL texture
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// send texture to GPU
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,width,height,0, GL_RGBA,GL_UNSIGNED_BYTE,pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	// free the image in RAM
	stbi_image_free(pixels);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

void Texture::bind(const unsigned int slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D,id);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

uint32_t Texture::getID()
{
	return id;
}

int Texture::getWidth()
{
	return width;
}

int Texture::getHeight()
{
	return height;
}

TextureMap Texture::format(float pX, float pY, float pWidth, float pHeight)
{
	return{
		pX / static_cast<float>(width),
		(pX + pWidth) / static_cast<float>(width),
		pY / static_cast<float>(height),
		(pY + pHeight) / static_cast<float>(height)
	};
}
