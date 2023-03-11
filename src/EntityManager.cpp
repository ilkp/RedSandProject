#include "core.h"

Entity EntityManager::make()
{
	std::unique_lock<std::mutex> lock(mutex);
	if (releasedEntities.size() == 0)
		return top++;
	Entity entity = *releasedEntities.begin();
	releasedEntities.erase(releasedEntities.begin());
	return entity;
}

void EntityManager::release(Entity entity)
{
	std::unique_lock<std::mutex> lock(mutex);
	releasedEntities.insert(entity);
	for (std::set<uint64_t>::reverse_iterator it = releasedEntities.rbegin(); it != releasedEntities.rend();)
	{
		if (*it != (top - 1))
			break;
		it = std::set<uint64_t>::reverse_iterator(releasedEntities.erase(std::next(it).base()));
		top--;
	}
}