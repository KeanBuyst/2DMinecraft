#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <SDL3/SDL_gpu.h>

namespace gl
{
    enum ShaderType
    {
        COMPUTE,
        VERTEX,
        FRAGMENT
    };

    class Shader
    {
    public:
        Shader(std::string name, ShaderType type);
        ~Shader();

        operator SDL_GPUShader*();
    private:
        const std::string name;

        SDL_GPUShader* obj;
    };

    SDL_GPUComputePipeline* GetComputePipline(std::string name);
    glm::mat4 GetOrthoMat();
}

