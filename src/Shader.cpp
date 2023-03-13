#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#pragma once

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	// get shader code from the files
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch(std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// compile shaders
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// link shader program
	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader()
{
	glDeleteProgram(id);
}

void Shader::use() const
{
	glUseProgram(id);
}

void Shader::setBool(const char* name, bool value) const					{ glUniform1i(glGetUniformLocation(id, name), (int)value); }
void Shader::setInteger(const char* name, int value) const					{ glUniform1i(glGetUniformLocation(id, name), value); }
void Shader::setFloat(const char* name, float value) const					{ glUniform1f(glGetUniformLocation(id, name), value); }
void Shader::setVector2f(const char* name, float x, float y) const			{ glUniform2f(glGetUniformLocation(id, name), x, y); }
void Shader::setVector2f(const char* name, const glm::vec2& value)const		{ glUniform2f(glGetUniformLocation(id, name), value.x, value.y); };
void Shader::setVector3f(const char* name, float x, float y, float z) const	{ glUniform3f(glGetUniformLocation(id, name), x, y, z); };
void Shader::setVector3f(const char* name, const glm::vec3& value) const	{ glUniform3f(glGetUniformLocation(id, name), value.x, value.y, value.z); };
void Shader::setVector4f(const char* name, float x, float y, float z, float w) const	{ glUniform4f(glGetUniformLocation(id, name), x, y, z, w); };
void Shader::setVector4f(const char* name, const glm::vec4& value) const	{ glUniform4f(glGetUniformLocation(id, name), value.x, value.y, value.z, value.w); };
void Shader::setMatrix4(const char* name, const glm::mat4& matrix) const	{ glUniformMatrix4fv(glGetUniformLocation(id, name), 1, false, glm::value_ptr(matrix)); };