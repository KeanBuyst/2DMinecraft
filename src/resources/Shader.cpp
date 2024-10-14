#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

std::string readFile(std::string filePath);

/*
* .vert - a vertex shader
* .geom - a geometry shader
* .frag - a fragment shader
* .comp - a compute shader
*/
GLuint GetShader(std::string path)
{
	std::string format = path.substr(path.size() - 5, 5);
	GLenum type;
	if (format == ".vert") type = GL_VERTEX_SHADER;
	else if (format == ".geom") type = GL_GEOMETRY_SHADER;
	else if (format == ".frag") type = GL_FRAGMENT_SHADER;
	else if (format == ".comp") type = GL_COMPUTE_SHADER;
	else
	{
		printf("Invalid shader format: %s", format.c_str());
		exit(-1);
	}

	// load code
	std::string data = readFile(path);

	const GLchar* code = data.c_str();

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);

	// error handling
	GLint success;
	GLchar infoLog[1024];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << format << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		exit(-1);
	}

	return shader;
}

std::string readFile(std::string filePath) {
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

ShaderProgram::ShaderProgram()
{
	ID = glCreateProgram();
	std::cout << "Creating Shader (" << ID << ")" << std::endl;
}

ShaderProgram::~ShaderProgram()
{
	std::cout << "Deleting Shader (" << ID << ")" << std::endl;
	glDeleteProgram(ID);
}

void ShaderProgram::bind(GLuint shader)
{
	if (built)
	{
		std::cerr << "Attempted to add shader to linked shader program" << std::endl;
		exit(-1);
	}
	glAttachShader(ID,shader);
	glDeleteShader(shader);
}

void ShaderProgram::build()
{
	glLinkProgram(ID);

	// check if linking was successful
	GLint success;
	GLchar infoLog[1024];
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		exit(-1);
	}

	built = true;
}

void ShaderProgram::use()
{
	glUseProgram(ID);
}

void ShaderProgram::sendMatrix(const GLchar* name, glm::mat4& matrix)
{
	glUniformMatrix4fv(locator(name), 1, GL_FALSE, &matrix[0][0]);
}

void ShaderProgram::useTexture(const GLchar* name,unsigned int slot)
{
	glUniform1i(locator(name), slot);
}

void ShaderProgram::sendVector2(const GLchar* name, glm::vec2& vector)
{
	glUniform2fv(locator(name), 1, &vector[0]);
}

GLint ShaderProgram::locator(const GLchar* name)
{
	try
	{
		return cache.at(name);
	}
	catch (std::out_of_range e)
	{
		GLint key = glGetUniformLocation(ID, name);
		cache[name] = key;
		return key;
	}
}