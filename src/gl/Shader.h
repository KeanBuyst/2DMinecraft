#pragma once

#include <string>
#include <unordered_map>
#include <glm.hpp>

namespace gl
{
    uint32_t GetShader(const std::string& path);

    class ShaderProgram
    {
    public:
        ShaderProgram();
        ~ShaderProgram();
        void bind(uint32_t shader) const;
        void use() const;
        void build();

        void sendMatrix(const char* name, glm::mat4& matrix);
        void useTexture(const char* name,unsigned int slot);
        void sendVector2(const char* name, glm::vec2& vector);
        void sendValue(const char* name,float value);

        uint32_t ID;
    private:
        bool built = false;
        std::unordered_map<const char*,int> cache;

        int locator(const char* name);
    };
}

