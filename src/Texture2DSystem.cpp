
#include "core.h"

Texture2DSystem::Texture2DSystem(uint64_t size) : size(size)
{
	data = new Component[size];
	top++;
	Texture2D& tex = data[0].texture;
	tex.attributes = Texture2DAttributes(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_RGB);
	tex.height = 2;
	tex.width = 2;
	tex.nChannels = 3;
	tex.pixels = new unsigned char[] {
		100, 0, 100,
		0, 0, 0,
		100, 0, 100,
		0, 0, 0
	};
	glGenTextures(1, &(data[0].id));
	glBindTexture(GL_TEXTURE_2D, data[0].id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex.attributes.textureWrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex.attributes.textureWrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex.attributes.minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex.attributes.magFilter);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		tex.width,
		tex.height,
		0,
		tex.attributes.format,
		GL_UNSIGNED_BYTE,
		tex.pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
}

Texture2DSystem::~Texture2DSystem()
{
	for (int i = 0; i < size; ++i)
		if (data[i].texture.pixels) delete[](data[i].texture.pixels);
	delete[](data);
}

Texture2D* Texture2DSystem::reserve(Entity entity)
{
	if (!indices.contains(entity))
		indices.insert(std::make_pair(entity, top++));
	return &data[indices[entity]].texture;
}

//void Texture2DSystem::release(Entity entity)
//{
//	mutex.lock();
//	if (!indices.contains(entity))
//	{
//		mutex.unlock();
//		return;
//	}
//	uint64_t index = indices[entity];
//	uint64_t topIndex = indices[top - 1];
//	if (index != topIndex)
//	{
//		memcpy(&data[index], &data[topIndex], sizeof(Texture2D));
//		indices[indices.find(topIndex)->first] = index;
//	}
//	indices.erase(entity);
//	top--;
//	mutex.unlock();
//}

Texture2D* Texture2DSystem::getTexture(Entity entity) const
{
	return &data[indices.at(entity)].texture;
}

unsigned int Texture2DSystem::getId(Entity entity) const
{
	return data[indices.at(entity)].id;
}

void Texture2DSystem::set(Entity entity, const Texture2D& texture)
{
	if (!indices.contains(entity))
		return;
	data[indices[entity]].texture = texture;
}

void Texture2DSystem::set(Entity entity, unsigned int id)
{
	if (!indices.contains(entity))
		return;
	data[indices[entity]].id = id;
}
