#pragma once

#include "Shader.h"

#include <concepts>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stdint.h>
#include <unordered_map>
#include <set>

typedef uint64_t Entity;

template<class T>
concept EntityComponent = requires(T a)
{
	{ a.clean() } -> std::convertible_to<void>;
};

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

struct VertexAttributes
{
	VertexAttributes(unsigned int dimensions, int strideBytes, int offsetBytes)
		: dimensions(dimensions), strideBytes(strideBytes), offsetBytes(offsetBytes) {};
	unsigned int dimensions;
	int strideBytes;
	int offsetBytes;
};

struct TextureBindLocation
{
	int textureLocation;
	Entity texture;
};

struct RenderUnit
{
	Entity glBuffer;
	Entity shader;
	std::vector<TextureBindLocation> textureBinds;
	std::set<Entity> vertices;
};

struct Mesh2D
{
	float* vertices = nullptr;
	unsigned int* triangles = nullptr;
	unsigned int verticesSize = 0;
	unsigned int trianglesSize = 0;
	Bounds2D bounds{ glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f) };

	Mesh2D& operator=(const Mesh2D& other)
	{
		if (this != &other)
		{
			if (verticesSize != other.verticesSize)
			{
				delete[] vertices;
				vertices = other.verticesSize ? new float[other.verticesSize] : nullptr;
				verticesSize = other.verticesSize;
			}
			memcpy(vertices, other.vertices, sizeof(float) * other.verticesSize);
			if (trianglesSize != other.trianglesSize)
			{
				delete[] triangles;
				triangles = other.trianglesSize ? new unsigned int[other.trianglesSize] : nullptr;
				trianglesSize = other.trianglesSize;
			}
			memcpy(triangles, other.triangles, sizeof(unsigned int) * other.trianglesSize);
		}
		return *this;
	}
	Mesh2D& operator=(Mesh2D&& other) noexcept
	{
		assert(this != &other);
		delete[]vertices;
		delete[]triangles;
		vertices = other.vertices;
		triangles = other.triangles;
		verticesSize = other.verticesSize;
		trianglesSize = other.trianglesSize;
		bounds.min = other.bounds.min;
		bounds.max = other.bounds.max;
		other.vertices = nullptr;
		other.triangles = nullptr;
		other.verticesSize = 0;
		other.trianglesSize = 0;
		other.bounds = { glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f) };
		return *this;
	}
	void clean()
	{
		if (vertices) delete[](vertices);
		if (triangles) delete[](triangles);
		vertices = nullptr;
		triangles = nullptr;
	}
}; static_assert(EntityComponent<Mesh2D>);

struct Texture2D
{
	unsigned char* pixels = nullptr;
	int width;
	int height;
	int nChannels;
	Texture2DAttributes attributes;

	void clean()
	{
		if (pixels) delete[](pixels);
	}
}; static_assert(EntityComponent<Texture2D>);

struct GLBuffers
{
	unsigned int vertexArrayObject = 0;
	unsigned int vertexBufferObject = 0;
	unsigned int triangleBuffer = 0;
	void clean() {}
}; static_assert(EntityComponent<GLBuffers>);

class EntityManager
{
public:
	Entity make();
	void release(Entity entity);

private:
	Entity top = 0;
	std::set<Entity> releasedEntities;
};

template<EntityComponent T>
class EntityComponentSystem
{
public:
	EntityComponentSystem(uint64_t size)
	{
		data = new T[size];
	}
	~EntityComponentSystem()
	{
		for (auto it = indices.begin(); it != indices.end(); ++it)
			data[it->second].clean();
		delete[](data);
	}
	EntityComponentSystem(const EntityComponentSystem& other) = delete;
	EntityComponentSystem(EntityComponentSystem&& other) = delete;
	EntityComponentSystem& operator=(const EntityComponentSystem other) = delete;
	EntityComponentSystem& operator=(EntityComponentSystem&& other) = delete;
	
	void reserve(Entity entity)
	{
		if (!indices.contains(entity))
			indices.insert(std::make_pair(entity, top++));
	}
	void release(Entity entity)
	{
		if (!indices.contains(entity))
			return;
		uint64_t index = indices[entity];
		uint64_t topIndex = indices[top - 1];
		data[indices[entity]].clean();
		if (index != topIndex)
		{
			memcpy(&data[index], &data[topIndex], sizeof(T));
			indices[indices.find(topIndex)->first] = index;
		}
		indices.erase(entity);
		top--;
	}
	T get(Entity entity) const
	{
		return data[indices.at(entity)];
	}
	void set(Entity entity, const T& component)
	{
		if (!indices.contains(entity))
			reserve(entity);
		data[indices[entity]] = component;
	}
	void set(Entity entity, T&& component)
	{
		if (!indices.contains(entity))
			reserve(entity);
		data[indices[entity]] = component;
	}

private:
	T* data;
	std::unordered_map<Entity, uint64_t> indices;
	uint64_t top = 0;
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

//class GLBufferSystem
//{
//public:
//	GLBufferSystem(uint64_t size) { data = new GLBuffers[size]; }
//	~GLBufferSystem() { delete[](data); }
//	GLBufferSystem(const GLBufferSystem& other) = delete;
//	GLBufferSystem(GLBufferSystem&& other) = delete;
//	GLBufferSystem& operator=(const GLBufferSystem other) = delete;
//	GLBufferSystem& operator=(GLBufferSystem&& other) = delete;
//
//	void reserve(Entity entity);
//	void release(Entity entity);
//	GLBuffers get(Entity entity) const;
//	void set(Entity entity, const GLBuffers& buffers);
//	void clean(Entity entity);
//
//private:
//	std::unordered_map<Entity, uint64_t> indices;
//	std::set<uint64_t> releasedIndices;
//	GLBuffers* data;
//	uint64_t top = 0;
//};

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

class ShaderSystem
{
public:
	ShaderSystem(uint64_t size) { data = new Shader[size]; }
	~ShaderSystem() { delete[](data); }
	ShaderSystem(const ShaderSystem& other) = delete;
	ShaderSystem(ShaderSystem&& other) = delete;
	ShaderSystem& operator=(const ShaderSystem other) = delete;
	ShaderSystem& operator=(ShaderSystem&& other) = delete;

	Shader* reserve(Entity entity);
	void release(Entity entity);
	Shader* get(Entity entity) const { return &data[indices.at(entity)]; }

private:
	std::unordered_map<Entity, uint64_t> indices;
	std::set<uint64_t> releasedIndices;
	Shader* data;
	uint64_t top = 0;

	void clean(Entity entity);
};

class RenderUnitSystem
{
public:
	RenderUnitSystem(uint64_t size) { data = new RenderUnit[size]; }
	~RenderUnitSystem() { delete[](data); }
	RenderUnitSystem(const RenderUnitSystem& other) = delete;
	RenderUnitSystem(RenderUnitSystem&& other) = delete;
	RenderUnitSystem& operator=(const RenderUnitSystem other) = delete;
	RenderUnitSystem& operator=(RenderUnitSystem&& other) = delete;

	RenderUnit* reserve(Entity entity);
	void release(Entity entity);
	RenderUnit* get(Entity entity) const;

private:
	std::unordered_map<Entity, uint64_t> indices;
	std::set<uint64_t> releasedIndices;
	RenderUnit* data;
	uint64_t top = 0;

	void clean(Entity entity);
};

struct RenderSystems2D
{
	EntityComponentSystem<GLBuffers>* glBufferSystem;
	ShaderSystem* shaderSystem;
	Texture2DSystem* textureSystem;
	Mesh2DSystem* meshSystem;
};