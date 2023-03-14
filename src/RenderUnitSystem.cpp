
#include "core.h"


RenderUnit* RenderUnitSystem::reserve(Entity entity)
{
	if (!indices.contains(entity))
		indices.insert(std::make_pair(entity, top++));
	return &data[indices[entity]];
}

void RenderUnitSystem::release(Entity entity)
{
	if (!indices.contains(entity))
		return;
	uint64_t index = indices[entity];
	uint64_t topIndex = indices[top - 1];
	//clean(entity);
	if (index != topIndex)
	{
		memcpy(&data[index], &data[topIndex], sizeof(Mesh2D));
		indices[indices.find(topIndex)->first] = index;
	}
	indices.erase(entity);
	top--;
}

RenderUnit* RenderUnitSystem::get(Entity entity) const
{
	return &data[indices.at(entity)];
}