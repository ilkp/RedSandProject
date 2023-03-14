#include "core.h"

void Mesh2DSystem::reserve(Entity entity)
{
	if (!indices.contains(entity))
		indices.insert(std::make_pair(entity, top++));
}

void Mesh2DSystem::release(Entity entity)
{
	if (!indices.contains(entity))
		return;
	uint64_t index = indices[entity];
	uint64_t topIndex = indices[top - 1];
	clean(entity);
	if (index != topIndex)
	{
		memcpy(&data[index], &data[topIndex], sizeof(Mesh2D));
		indices[indices.find(topIndex)->first] = index;
	}
	indices.erase(entity);
	top--;
}

Mesh2D* Mesh2DSystem::get(Entity entity) const
{
	return &data[indices.at(entity)];
}

void Mesh2DSystem::copy(Entity entity, const Mesh2D& mesh)
{
	if (!indices.contains(entity))
		reserve(entity);
	Mesh2D& internalMesh = data[indices[entity]];
	copy(entity, mesh.vertices, mesh.verticesSize, mesh.triangles, mesh.trianglesSize);
	internalMesh.bounds.min = mesh.bounds.min;
	internalMesh.bounds.max = mesh.bounds.max;
}

void Mesh2DSystem::copy(Entity entity,
	const float* vertices, unsigned int verticesSize,
	const unsigned int* triangles, unsigned int trianglesSize)
{
	if (!indices.contains(entity))
		reserve(entity);
	Mesh2D& internalMesh = data[indices[entity]];
	internalMesh.verticesSize = verticesSize;
	internalMesh.trianglesSize = trianglesSize;
	internalMesh.vertices = new float[internalMesh.verticesSize];
	internalMesh.triangles = new unsigned int[internalMesh.trianglesSize];
	memcpy(internalMesh.vertices, vertices, sizeof(vertices[0]) * verticesSize);
	memcpy(internalMesh.triangles, triangles, sizeof(triangles[0]) * trianglesSize);
}

void Mesh2DSystem::calculateBounds(Entity entity, unsigned int stride)
{
	if (!indices.contains(entity))
		return;
	Mesh2D& mesh = data[indices[entity]];
	glm::vec2 min(mesh.vertices[0]);
	glm::vec2 max(mesh.vertices[0]);
	for (unsigned i = 0; i < mesh.verticesSize / stride; i += stride)
	{
		const float* vertex = &mesh.vertices[i];
		if (vertex[0] < min.x) min.x = vertex[0];
		if (vertex[1] < min.y) min.y = vertex[1];
		if (vertex[0] > max.x) max.x = vertex[0];
		if (vertex[1] > max.y) max.y = vertex[1];
	}
	mesh.bounds.min = min;
	mesh.bounds.max = max;
}

void Mesh2DSystem::clean(Entity entity)
{
	if (!indices.contains(entity))
		return;
	data[indices[entity]].clean();
}