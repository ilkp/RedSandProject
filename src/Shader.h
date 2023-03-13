#pragma once

#include <glm/glm.hpp>

class Shader
{
public:
	unsigned int id;

	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();
	void use() const;
    void setBool(const char* name, bool value) const;
    void setInteger(const char* name, int value) const;
    void setFloat(const char* name, float value) const;
    void setVector2f(const char* name, float x, float y) const;
    void setVector2f(const char* name, const glm::vec2& value) const;
    void setVector3f(const char* name, float x, float y, float z) const;
    void setVector3f(const char* name, const glm::vec3& value) const;
    void setVector4f(const char* name, float x, float y, float z, float w) const;
    void setVector4f(const char* name, const glm::vec4& value) const;
    void setMatrix4(const char* name, const glm::mat4& matrix) const;
};