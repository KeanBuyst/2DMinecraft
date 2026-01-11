#pragma once
#include <cstdint>

#include <SDL3/SDL_gpu.h>

#include "Frame.h"

namespace gl
{
	class Texture 
	{
	private:
		static SDL_GPUSampler* sampler;
		SDL_GPUTexture* texture;
		int width, height;
	public:
		static SDL_GPUSampler* GetSampler();
		explicit Texture(const char* path);
		~Texture();

		operator SDL_GPUTexture*();

		int getWidth();
		int getHeight();

		TextureMap format(float pX, float pY, float pWidth, float pHeight);
	};
}

