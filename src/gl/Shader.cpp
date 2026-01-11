#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/ext/matrix_clip_space.hpp>

#include "../Application.h"
#include "../libs/SDL_shadercross.h"

using namespace gl;

static std::string PATH = "../shaders/";

std::string readFile(const std::string& filePath) {
	std::ifstream file;
	std::stringstream buffer;

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		file.open(filePath);
		buffer << file.rdbuf();
		file.close();
	}
	catch (std::ifstream::failure& e) {
		std::cerr << e.what() << "\n" << e.code() << std::endl;
	}
	return buffer.str();
}

SDL_PropertiesID GetProperties()
{
	// only create properties once
	static SDL_PropertiesID id = 0;
	if (id == 0)
	{
		id = SDL_CreateProperties();
		SDL_SetBooleanProperty(id,SDL_SHADERCROSS_PROP_SHADER_DEBUG_ENABLE_BOOLEAN,true);
		SDL_SetStringProperty(id,"SDL.shadercross.hlsl.profile","cs_6_0");
	}
	return id;
}

SDL_ShaderCross_SPIRV_Info LoadShaderFile(std::string name, SDL_ShaderCross_ShaderStage stage)
{
	size_t codeSize;
	std::string file = PATH + name + ".hlsl";
	const char* code = reinterpret_cast<const char*>(SDL_LoadFile(file.c_str(),&codeSize));

	SDL_PropertiesID props = GetProperties();
	SDL_SetStringProperty(props,SDL_PROP_GPU_SHADER_CREATE_NAME_STRING,name.c_str());

	std::string worldWidth = std::to_string(world::WORLD_WIDTH * world::CHUNK_SIZE) + ".0f";
	std::string worldHeight = std::to_string(world::WORLD_HEIGHT * world::CHUNK_SIZE) + ".0f";
	std::string chunkSize = std::to_string(world::CHUNK_SIZE) + ".0f";
	std::string blockSize = std::to_string(BLOCK_SIZE);

	SDL_ShaderCross_HLSL_Define shaderDefines[] = {
		{ const_cast<char*>("WIDTH"),const_cast<char*>(worldWidth.c_str()) },
		{ const_cast<char*>("HEIGHT"),const_cast<char*>(worldHeight.c_str()) },
		{const_cast<char*>("CHUNK_SIZE"),const_cast<char*>(chunkSize.c_str()) },
		{const_cast<char*>("BLOCK_SIZE"),const_cast<char*>(blockSize.c_str()) },
		{NULL,NULL}
	};

	SDL_ShaderCross_HLSL_Info hlslInfo = {
		code,
		"main",
		PATH.c_str(),
		shaderDefines,
		stage,
		props
	};

	size_t byteSize;
	Uint8* bytes = reinterpret_cast<Uint8*>(SDL_ShaderCross_CompileSPIRVFromHLSL(&hlslInfo,&byteSize));
	if (!bytes)
	{
		SDL_Log("Error: SDL_ShaderCross_CompileSPIRVFromHLSL\n%s", SDL_GetError());
		throw "Error: SDL_ShaderCross_CompileSPIRVFromHLSL";
	}

	return {
		bytes,
		byteSize,
		"main",
		stage,
		props
	};
}

Shader::Shader(std::string name, ShaderType type) : name(name)
{
	// TODO cache compiled shaders
	// if (SDL_CreateDirectory("cache"))
	// {
	// 	SDL_Log("Error: SDL_CreateDirectory -> %s", SDL_GetError());
	// 	throw "Error: SDL_CreateDirectory";
	// }

	SDL_ShaderCross_ShaderStage stage;
	switch (type)
	{
	case VERTEX:
		stage = SDL_SHADERCROSS_SHADERSTAGE_VERTEX;
		break;
	case FRAGMENT:
		stage = SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT;
		break;
	default:
		throw "Error: ShaderType unmapped";
	}

	SDL_ShaderCross_SPIRV_Info info = LoadShaderFile(name,stage);

	SDL_ShaderCross_GraphicsShaderMetadata* meta = SDL_ShaderCross_ReflectGraphicsSPIRV(
		info.bytecode,
		info.bytecode_size,
		0
	);
	if (!meta)
	{
		SDL_Log("Error: SDL_ShaderCross_ReflectGraphicsSPIRV\n%s",SDL_GetError());
		SDL_free(const_cast<Uint8*>(info.bytecode));
		throw "Error: SDL_ShaderCross_ReflectGraphicsSPIRV";
	}

	obj = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(Application::GPU_DEVICE,&info,&meta->resource_info,0);
	if (!obj)
	{
		SDL_Log("Error: SDL_ShaderCross_CompileGraphicsShaderFromSPIRV\n%s", SDL_GetError());
		throw "Error: SDL_ShaderCross_CompileGraphicsShaderFromSPIRV";
	}
	SDL_free(meta);
	SDL_free(const_cast<Uint8*>(info.bytecode));
	SDL_Log("Creating Shader (%s)", name.c_str());
}

Shader::~Shader()
{
	SDL_Log("Deleting Shader (%s)", name.c_str());
	SDL_ReleaseGPUShader(Application::GPU_DEVICE,obj);
}

Shader::operator SDL_GPUShader*()
{
	return obj;
}

SDL_GPUComputePipeline* gl::GetComputePipline(std::string name)
{
	SDL_ShaderCross_SPIRV_Info info = LoadShaderFile(name,SDL_SHADERCROSS_SHADERSTAGE_COMPUTE);

	SDL_ShaderCross_ComputePipelineMetadata* meta = SDL_ShaderCross_ReflectComputeSPIRV(
			info.bytecode,
			info.bytecode_size,
			0
		);
	if (!meta)
	{
		SDL_Log("Error: SDL_ShaderCross_ReflectComputeSPIRV\n%s",SDL_GetError());
		SDL_free(const_cast<Uint8*>(info.bytecode));
		throw "Error: SDL_ShaderCross_ReflectComputeSPIRV";
	}

	SDL_GPUComputePipeline* pipeline = SDL_ShaderCross_CompileComputePipelineFromSPIRV(Application::GPU_DEVICE,&info,meta,0);
	if (!pipeline)
	{
		SDL_Log("Error: SDL_ShaderCross_CompileComputePipelineFromSPIRV\n%s",SDL_GetError());
		SDL_free(meta);
		SDL_free(const_cast<Uint8*>(info.bytecode));
		throw "Error: SDL_ShaderCross_CompileComputePipelineFromSPIRV";
	}
	SDL_free(meta);
	SDL_free(const_cast<Uint8*>(info.bytecode));
	return pipeline;
}

glm::mat4 gl::GetOrthoMat()
{
	const float aspect = (static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT)) * VIEW_SCALE;
	VIEW_SIZE.x = aspect;
	glm::mat4 mat = glm::ortho<float>(-aspect, aspect, -VIEW_SCALE, VIEW_SCALE,-5.0f,5.0f);
	return mat;
}
