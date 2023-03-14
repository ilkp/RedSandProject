//
//#include "core.h"
//
//namespace Render { void deleteBuffers(const GLBuffers& buffers); }
//
//void GLBufferSystem::reserve(Entity entity)
//{
//	if (!indices.contains(entity))
//		indices.insert(std::make_pair(entity, top++));
//}
//
//void GLBufferSystem::release(Entity entity)
//{
//	if (!indices.contains(entity))
//		return;
//	GLBuffers buffers = data[indices[entity]];
//	Render::deleteBuffers(buffers);
//	uint64_t index = indices[entity];
//	uint64_t topIndex = indices[top - 1];
//	if (index != topIndex)
//	{
//		memcpy(&data[index], &data[topIndex], sizeof(GLBuffers));
//		indices[indices.find(topIndex)->first] = index;
//	}
//	indices.erase(entity);
//	top--;
//}
//
//GLBuffers GLBufferSystem::get(Entity entity) const
//{
//	if (!indices.contains(entity))
//		return GLBuffers();
//	GLBuffers buffers = GLBuffers(data[indices.at(entity)]);
//	return buffers;
//}
//
//void GLBufferSystem::set(Entity entity, const GLBuffers& buffers)
//{
//	if (!indices.contains(entity))
//		return;
//	data[indices[entity]] = buffers;
//}
//
//void GLBufferSystem::clean(Entity entity)
//{
//	if (!indices.contains(entity))
//		return;
//	GLBuffers buffers = data[indices[entity]];
//	glDeleteVertexArrays(1, &buffers.vertexArrayObject);
//	glDeleteBuffers(1, &buffers.vertexBufferObject);
//	glDeleteBuffers(1, &buffers.triangleBuffer);
//}