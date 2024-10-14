#pragma once

#include <glew.h>
#include <string>
#include <unordered_map>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Texture.h"

GLuint GetShader(std::string path);

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();
	void bind(GLuint shader);
	void use();
	void build();

	void sendMatrix(const GLchar* name, glm::mat4& matrix);
	void useTexture(const GLchar* name,unsigned int slot);
	void sendVector2(const GLchar* name, glm::vec2& vector);

	GLuint ID;
private:
	bool built = false;
	std::unordered_map<const GLchar*,GLint> cache;

	GLint locator(const GLchar* name);
};

