#pragma once
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

struct Texture2D
{
	int width;
	int height;
	int nChannels;
	unsigned int id;
};

struct GLBuffers
{
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
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
};

class Texture2DSystem
{
public:
	Texture2DSystem(uint64_t size) { data = new Texture2D[size]; }
	~Texture2DSystem() { delete[](data); }
	Texture2DSystem(const Texture2DSystem& other) = delete;
	Texture2DSystem(Texture2DSystem&& other) = delete;
	Texture2DSystem& operator=(const Texture2DSystem other) = delete;
	Texture2DSystem& operator=(Texture2DSystem&& other) = delete;

	void reserve(Entity entity);
	//void release(Entity entity);
	std::optional<unsigned int> get(Entity entity);
	bool initialize(Entity entity, std::string filePath, int textureWrapS, int textureWrapT, int minFilter, int magFilter, int format);

private:
	std::unordered_map<Entity, uint64_t> indices;
	std::set<uint64_t> releasedIndices;
	Texture2D* data;
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
	bool initialize(Entity entity, std::vector<VertexAttributes> attributes, std::vector<float> vertices, std::vector<unsigned int> triangles);
	void clean(Entity entity);

private:
	std::unordered_map<Entity, uint64_t> indices;
	std::set<uint64_t> releasedIndices;
	GLBuffers* data;
	uint64_t top = 0;
	std::mutex mutex;
};