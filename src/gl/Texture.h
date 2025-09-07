#pragma once
#include <cstdint>

#include <glm.hpp>

namespace gl
{
	class Texture 
	{
	public:
		explicit Texture(const char* path);
		~Texture();

		void bind(unsigned int slot) const;

		void unbind();

		uint32_t ID;
		int width, height;
	};

	class AtlasTexture : public Texture
	{
	public:
		explicit AtlasTexture(const char* path) : Texture(path) {};
		// format: x,y,width,height
		[[nodiscard]] glm::vec4 getTexel(glm::vec4 pixel_rect) const;
		static glm::vec4 getTexel(uint8_t index);
	};
}

