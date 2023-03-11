#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stdint.h>
#include <unordered_map>
#include <set>
#include <mutex>
#include <optional>

typedef uint64_t Entity;

struct Bounds3D
{
	glm::vec3 min;
	glm::vec3 max;
};

struct Mesh3D
{
	int verticesLength;
	int trianglesLength;
	int uvsLength;
	Bounds3D bounds;
	float* data;
	glm::fvec3* vertices;
	int* triangles;
	glm::fvec2* uvs;
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
	unsigned char* pixels;
	int width;
	int height;
	int nChannels;
	Texture2DAttributes attributes;
};

struct GLBuffers
{
	unsigned int vertexArrayObject;
	unsigned int vertexBufferObject;
	unsigned int elementBufferObject;
};

struct VertexAttributes
{
	unsigned int dimensions;
	int stride;
	size_t offset;
};

class EntityManager
{
public:
	Entity make();
	void release(Entity entity);

private:
	Entity top = 0;
	std::set<Entity> releasedEntities;
	std::mutex mutex;
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

	void reserve(Entity entity);
	//void release(Entity entity);
	std::optional<Texture2D> getTexture(Entity entity);
	unsigned int getId(Entity entity);
	void set(Entity entity, Texture2D texture);
	void load(Entity entity);

private:
	uint64_t size;
	std::unordered_map<Entity, uint64_t> indices;
	std::set<uint64_t> releasedIndices;
	std::pair<Texture2D, unsigned int>* data;
	uint64_t top = 0;
	std::mutex mutex;
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
	std::optional<GLBuffers> get(Entity entity);
	bool load(Entity entity, std::vector<VertexAttributes> attributes, std::vector<float> vertices, std::vector<unsigned int> triangles);
	void clean(Entity entity);

private:
	std::unordered_map<Entity, uint64_t> indices;
	std::set<uint64_t> releasedIndices;
	GLBuffers* data;
	uint64_t top = 0;
	std::mutex mutex;
};

class MeshSystem
{
public:
	MeshSystem(uint64_t size) { data = new GLBuffers[size]; }
	~MeshSystem() { delete[](data); }
	MeshSystem(const MeshSystem& other) = delete;
	MeshSystem(MeshSystem&& other) = delete;
	MeshSystem& operator=(const MeshSystem other) = delete;
	MeshSystem& operator=(MeshSystem&& other) = delete;

	void reserve(Entity entity);
	void release(Entity entity);
	std::optional<GLBuffers> get(Entity entity);
	bool initialize(Entity entity, std::vector<VertexAttributes> attributes, std::vector<float> vertices, std::vector<unsigned int> triangles);
	void clean(Entity entity);

private:
	std::unordered_map<Entity, uint64_t> indices;
	std::set<uint64_t> releasedIndices;
	GLBuffers* data;
	uint64_t top = 0;
	std::mutex mutex;
};