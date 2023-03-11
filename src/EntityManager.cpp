#include "core.h"

Entity EntityManager::make()
{
	if (releasedEntities.size() == 0)
		return top++;
	Entity entity = *releasedEntities.begin();
	releasedEntities.erase(releasedEntities.begin());
	return entity;
}

void EntityManager::release(Entity entity)
{
	releasedEntities.insert(entity);
	for (std::set<uint64_t>::reverse_iterator it = releasedEntities.rbegin(); it != releasedEntities.rend();)
	{
		if (*it != (top - 1))
			break;
		it = std::set<uint64_t>::reverse_iterator(releasedEntities.erase(std::next(it).base()));
		top--;
	}
}