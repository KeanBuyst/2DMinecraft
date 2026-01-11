#include "Texture.h"

#include <iostream>
#include "../libs/stb_image.h"

#include "../Application.h"


using namespace gl;

SDL_GPUSampler* Texture::sampler = nullptr;

SDL_GPUSampler* Texture::GetSampler()
{
	if (!sampler)
	{
		SDL_GPUSamplerCreateInfo createInfo = {};
		createInfo.min_filter = SDL_GPU_FILTER_NEAREST;
		createInfo.mag_filter = SDL_GPU_FILTER_NEAREST;
		createInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
		createInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
		createInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
		createInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

		sampler = SDL_CreateGPUSampler(Application::GPU_DEVICE, &createInfo);
		if (!sampler)
		{
			SDL_Log("Error: SDL_CreateGPUSampler\n%s", SDL_GetError());
			throw "Error: SDL_CreateGPUSampler";
		}
	}
	return sampler;
}

Texture::Texture(const char* path)
{
	int channels;
	unsigned char* data = stbi_load(path, &width, &height, &channels, 4);
	if (!data)
	{
		SDL_Log("Error loading image found at path: %s\n%s", path, stbi_failure_reason());
		throw "Error loading image";
	}

	SDL_GPUTextureCreateInfo texture_info = {
		SDL_GPU_TEXTURETYPE_2D,
		SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
		SDL_GPU_TEXTUREUSAGE_SAMPLER,
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height),
		1,
		1,
		SDL_GPU_SAMPLECOUNT_1,
		0
	};

	texture = SDL_CreateGPUTexture(Application::GPU_DEVICE,&texture_info);
	SDL_SetGPUTextureName(Application::GPU_DEVICE,texture,path);

	Uint32 dataSize = width * height * 4; // 4 bytes per pixel (RGBA)

	SDL_GPUTransferBufferCreateInfo transferInfo = {
		SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		dataSize,
		0
	};

	SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(Application::GPU_DEVICE, &transferInfo);

	// --- STEP 3: Map & Copy (Load the Truck) ---
	void* mapPtr = SDL_MapGPUTransferBuffer(Application::GPU_DEVICE, transferBuffer, false);
	memcpy(mapPtr, data, dataSize);
	SDL_UnmapGPUTransferBuffer(Application::GPU_DEVICE, transferBuffer);

	stbi_image_free(data);

	// --- STEP 4: The Copy Pass (Drive the Truck to VRAM) ---
	SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(Application::GPU_DEVICE);
	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

	SDL_GPUTextureTransferInfo source = {};
	source.transfer_buffer = transferBuffer;
	source.offset = 0;
	source.pixels_per_row = width;
	source.rows_per_layer = height;

	SDL_GPUTextureRegion destination = {};
	destination.texture = texture;
	destination.w = width;
	destination.h = height;
	destination.d = 1;

	SDL_UploadToGPUTexture(copyPass, &source, &destination, false);
	SDL_EndGPUCopyPass(copyPass);
	SDL_SubmitGPUCommandBuffer(cmd);
	SDL_ReleaseGPUTransferBuffer(Application::GPU_DEVICE, transferBuffer);
}

Texture::~Texture()
{
	SDL_ReleaseGPUTexture(Application::GPU_DEVICE,texture);
	// if we are destroying any texture we should also destroy the sampler
	if (sampler)
	{
		SDL_ReleaseGPUSampler(Application::GPU_DEVICE,sampler);
		sampler = nullptr;
	}
}

Texture::operator SDL_GPUTexture*()
{
	return texture;
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
