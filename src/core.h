#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stdint.h>
#include <unordered_map>
#include <set>

typedef uint64_t Entity;

struct Bounds2D
{
	glm::vec2 min;
	glm::vec2 max;
};

struct Bounds3D
{
	glm::vec3 min;
	glm::vec3 max;
};

struct Mesh2D
{
	float* vertices;
	unsigned int* triangles;
	unsigned int verticesLength;
	unsigned int trianglesLength;
	Bounds2D bounds;

	void clean()
	{
		delete[](vertices);
		delete[](triangles);
	}
};

struct Texture2DAttributes
{
	Texture2DAttributes() {};
	Texture2DAttributes(int textureWrapS, int textureWrapT, int minFilter, int magFilter, int format)
		: textureWrapS(textureWrapS), textureWrapT(textureWrapT), minFilter(minFilter), magFilter(magFilter), format(format) {}
	int textureWrapS = GL_REPEAT;
	int textureWrapT = GL_REPEAT;
	int minFilter = GL_LINEAR;
	int magFilter = GL_LINEAR;
	int format = GL_RGB;
};

struct Texture2D
{
	unsigned char* pixels = nullptr;
	int width;
	int height;
	int nChannels;
	Texture2DAttributes attributes;
};

struct GLBuffers
{
	unsigned int vertexArrayObject = 0;
	unsigned int vertexBufferObject = 0;
	unsigned int triangleBuffer = 0;
};

struct VertexAttributes
{
	VertexAttributes(unsigned int dimensions, int strideBytes, int offsetBytes)
		: dimensions(dimensions), strideBytes(strideBytes), offsetBytes(offsetBytes) {};
	unsigned int dimensions;
	int strideBytes;
	int offsetBytes;
};

class EntityManager
{
public:
	Entity make();
	void release(Entity entity);

private:
	Entity top = 0;
	std::set<Entity> releasedEntities;
};

class Texture2DSystem
{
public:
	Texture2DSystem(uint64_t size);
	~Texture2DSystem();
	Texture2DSystem(const Texture2DSystem& other) = delete;
	Texture2DSystem(Texture2DSystem&& other) = delete;
	Texture2DSystem& operator=(const Texture2DSystem other) = delete;
	Texture2DSystem& operator=(Texture2DSystem&& other) = delete;

	Texture2D* reserve(Entity entity);
	//void release(Entity entity);
	Texture2D* getTexture(Entity entity) const;
	unsigned int getId(Entity entity) const;
	void set(Entity entity, const Texture2D& texture);
	void set(Entity entity, unsigned int id);

private:
	struct Component
	{
		Texture2D texture;
		unsigned id;
	};
	uint64_t size;
	std::unordered_map<Entity, uint64_t> indices;
	std::set<uint64_t> releasedIndices;
	Component* data;
	uint64_t top = 0;
};

class GLBufferSystem
{
public:
	GLBufferSystem(uint64_t size) { data = new GLBuffers[size]; }
	~GLBufferSystem() { delete[](data); }
	GLBufferSystem(const GLBufferSystem& other) = delete;
	GLBufferSystem(GLBufferSystem&& other) = delete;
	GLBufferSystem& operator=(const GLBufferSystem other) = delete;
	GLBufferSystem& operator=(GLBufferSystem&& other) = delete;

	void reserve(Entity entity);
	void release(Entity entity);
	GLBuffers get(Entity entity) const;
	void set(Entity entity, const GLBuffers& buffers);
	void clean(Entity entity);

private:
	std::unordered_map<Entity, uint64_t> indices;
	std::set<uint64_t> releasedIndices;
	GLBuffers* data;
	uint64_t top = 0;
};

class Mesh2DSystem
{
public:
	Mesh2DSystem(uint64_t size) { data = new Mesh2D[size]; }
	~Mesh2DSystem() { delete[](data); }
	Mesh2DSystem(const Mesh2DSystem& other) = delete;
	Mesh2DSystem(Mesh2DSystem&& other) = delete;
	Mesh2DSystem& operator=(const Mesh2DSystem other) = delete;
	Mesh2DSystem& operator=(Mesh2DSystem&& other) = delete;

	void reserve(Entity entity);
	void release(Entity entity);
	Mesh2D* get(Entity entity) const;
	void copy(Entity entity, const Mesh2D& mesh);
	void copy(Entity entity,
		const float* vertices, unsigned int verticesSize,
		const unsigned int* triangles, unsigned int trianglesSize);
	void calculateBounds(Entity entity, unsigned int stride);

private:
	std::unordered_map<Entity, uint64_t> indices;
	std::set<uint64_t> releasedIndices;
	Mesh2D* data;
	uint64_t top = 0;

	void clean(Entity entity);
};
