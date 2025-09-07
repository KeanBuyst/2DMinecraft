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
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

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
	glDeleteTextures(1, &ID);
}

void Texture::bind(const unsigned int slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D,ID);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

glm::vec4 AtlasTexture::getTexel(const glm::vec4 pixel_rect) const
{
	const float x = pixel_rect.x / static_cast<float>(width);
	const float y = pixel_rect.y / static_cast<float>(height);
	const float w = pixel_rect.z / static_cast<float>(width);
	const float h = pixel_rect.w / static_cast<float>(height);
	return {x,y,w,h};
}

glm::vec4 AtlasTexture::getTexel(const uint8_t index)
{
	auto tileTexPos = glm::vec2(static_cast<float>(index % 16) / 16.0,static_cast<float>(index / 16) / 16.0);
	return {tileTexPos,0.0625f,0.0625f};
}
