#include "core.h"
#include <glad/glad.h>

void GLBufferSystem::reserve(Entity entity)
{
	mutex.lock();
	if (!indices.contains(entity))
		indices.insert(std::make_pair(entity, top++));
	mutex.unlock();
}

void GLBufferSystem::release(Entity entity)
{
	mutex.lock();
	if (!indices.contains(entity))
	{
		mutex.unlock();
		return;
	}
	GLBuffers buffers = data[indices[entity]];
	glDeleteVertexArrays(1, &buffers.VAO);
	glDeleteBuffers(1, &buffers.VBO);
	glDeleteBuffers(1, &buffers.EBO);
	uint64_t index = indices[entity];
	uint64_t topIndex = indices[top - 1];
	if (index != topIndex)
	{
		memcpy(&data[index], &data[topIndex], sizeof(GLBuffers));
		indices[indices.find(topIndex)->first] = index;
	}
	indices.erase(entity);
	top--;
	mutex.unlock();
}

std::optional<GLBuffers> GLBufferSystem::get(Entity entity)
{
	mutex.lock();
	if (!indices.contains(entity))
	{
		mutex.unlock();
		return {};
	}
	GLBuffers buffers = GLBuffers(data[indices[entity]]);
	mutex.unlock();
	return buffers;
}

bool GLBufferSystem::initialize(Entity entity, std::vector<VertexAttributes> attributes, std::vector<float> vertices, std::vector<unsigned int> triangles)
{
	mutex.lock();
	if (!indices.contains(entity))
	{
		mutex.unlock();
		return false;
	}
	GLBuffers& buffers = data[indices[entity]]; 
	glGenVertexArrays(1, &buffers.VAO);
	glGenBuffers(1, &buffers.VBO);
	glGenBuffers(1, &buffers.EBO);
	glBindVertexArray(buffers.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles[0]) * triangles.size(), triangles.data(), GL_STATIC_DRAW);

	for (int i = 0; i < attributes.size(); ++i)
	{
		glVertexAttribPointer(i, attributes[i].dimensions, GL_FLOAT, GL_FALSE, attributes[i].stride, (void*)attributes[i].offset);
		glEnableVertexAttribArray(i);
	}
	glBindVertexArray(0);
	mutex.unlock();
	return true;
}

void GLBufferSystem::clean(Entity entity)
{
	mutex.lock();
	if (!indices.contains(entity))
	{
		mutex.unlock();
		return;
	}
	GLBuffers buffers = data[indices[entity]];
	glDeleteVertexArrays(1, &buffers.VAO);
	glDeleteBuffers(1, &buffers.VBO);
	glDeleteBuffers(1, &buffers.EBO);
	mutex.unlock();
}