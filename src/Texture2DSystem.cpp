
#include "core.h"
#include <iostream>
#include "stb_image.h"

#include <glad/glad.h>

void Texture2DSystem::reserve(Entity entity)
{
	mutex.lock();
	if (!indices.contains(entity))
		indices.insert(std::make_pair(entity, top++));
	mutex.unlock();
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

std::optional<unsigned int> Texture2DSystem::get(Entity entity)
{
	mutex.lock();
	if (!indices.contains(entity))
	{
		mutex.unlock();
		return {};
	}
	unsigned int id = data[indices[entity]].id;
	mutex.unlock();
	return id;
}

bool Texture2DSystem::initialize(Entity entity, std::string filePath, int textureWrapS, int textureWrapT, int minFilter, int magFilter, int format)
{
	mutex.lock();
	if (!indices.contains(entity))
	{
		mutex.unlock();
		return false;
	}
	bool success = false;
	Texture2D& tex = data[indices[entity]];
	glGenTextures(1, &tex.id);
	glBindTexture(GL_TEXTURE_2D, tex.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	unsigned char* file = stbi_load(filePath.c_str(), &tex.width, &tex.height, &tex.nChannels, 0);
	if (file)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex.width, tex.height, 0, format, GL_UNSIGNED_BYTE, file);
		glGenerateMipmap(GL_TEXTURE_2D);
		success = true;
	}
	if (!success)
		std::cout << "Failed to load texture" << std::endl;

	mutex.unlock();
	stbi_image_free(file);
	return success;
}
