#include "core.h"
#include <glad/glad.h>

void GLBufferSystem::reserve(Entity entity)
{
	std::unique_lock<std::mutex> lock(mutex);
	if (!indices.contains(entity))
		indices.insert(std::make_pair(entity, top++));
}

void GLBufferSystem::release(Entity entity)
{
	std::unique_lock<std::mutex> lock(mutex);
	if (!indices.contains(entity))
		return;
	GLBuffers buffers = data[indices[entity]];
	glDeleteVertexArrays(1, &buffers.vertexArrayObject);
	glDeleteBuffers(1, &buffers.vertexBufferObject);
	glDeleteBuffers(1, &buffers.elementBufferObject);
	uint64_t index = indices[entity];
	uint64_t topIndex = indices[top - 1];
	if (index != topIndex)
	{
		memcpy(&data[index], &data[topIndex], sizeof(GLBuffers));
		indices[indices.find(topIndex)->first] = index;
	}
	indices.erase(entity);
	top--;
}

std::optional<GLBuffers> GLBufferSystem::get(Entity entity)
{
	std::unique_lock<std::mutex> lock(mutex);
	if (!indices.contains(entity))
		return {};
	GLBuffers buffers = GLBuffers(data[indices[entity]]);
	return buffers;
}

bool GLBufferSystem::load(Entity entity, std::vector<VertexAttributes> attributes, std::vector<float> vertices, std::vector<unsigned int> triangles)
{
	std::unique_lock<std::mutex> lock(mutex);
	if (!indices.contains(entity))
		return false;
	GLBuffers& buffers = data[indices[entity]]; 
	glGenVertexArrays(1, &buffers.vertexArrayObject);
	glGenBuffers(1, &buffers.vertexBufferObject);
	glGenBuffers(1, &buffers.elementBufferObject);
	glBindVertexArray(buffers.vertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, buffers.vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles[0]) * triangles.size(), triangles.data(), GL_STATIC_DRAW);
	for (int i = 0; i < attributes.size(); ++i)
	{
		glVertexAttribPointer(i, attributes[i].dimensions, GL_FLOAT, GL_FALSE, attributes[i].stride, (void*)attributes[i].offset);
		glEnableVertexAttribArray(i);
	}
	glBindVertexArray(0);
	return true;
}

void GLBufferSystem::clean(Entity entity)
{
	std::unique_lock<std::mutex> lock(mutex);
	if (!indices.contains(entity))
		return;
	GLBuffers buffers = data[indices[entity]];
	glDeleteVertexArrays(1, &buffers.vertexArrayObject);
	glDeleteBuffers(1, &buffers.vertexBufferObject);
	glDeleteBuffers(1, &buffers.elementBufferObject);
}