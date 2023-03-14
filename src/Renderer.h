#pragma once

#include "core.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

/*
* bufferit
* vertexit & texcoordww
* shader
* MVP-matrix
* ikkuna
*/ 

namespace Render
{
	void draw(const RenderSystems2D& renderSystems, const RenderUnit* renderUnit)
	{
		for (auto it = renderUnit->textureBinds.begin(); it != renderUnit->textureBinds.end(); ++it)
		{
			glActiveTexture(it->textureLocation);
			glBindTexture(GL_TEXTURE_2D, renderSystems.textureSystem->getId(it->texture));
		}
		renderSystems.shaderSystem->get(renderUnit->shader)->use();
		glBindVertexArray(renderSystems.glBufferSystem->get(renderUnit->glBuffer).vertexArrayObject);
		for (auto it = renderUnit->vertices.begin(); it != renderUnit->vertices.end(); ++it)
			glDrawElements(GL_TRIANGLES, renderSystems.meshSystem->get(*it)->trianglesSize, GL_UNSIGNED_INT, 0);
	}

	GLBuffers bindBuffers(
		std::vector<VertexAttributes> attributes,
		float* vertices, unsigned int verticesSize,
		unsigned* triangles, unsigned int trianglesSize)
	{
		GLBuffers buffers;
		glGenVertexArrays(1, &buffers.vertexArrayObject);
		glGenBuffers(1, &buffers.vertexBufferObject);
		glGenBuffers(1, &buffers.triangleBuffer);
		glBindVertexArray(buffers.vertexArrayObject);

		glBindBuffer(GL_ARRAY_BUFFER, buffers.vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * verticesSize, vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.triangleBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles[0]) * trianglesSize, triangles, GL_STATIC_DRAW);

		for (int i = 0; i < attributes.size(); ++i)
		{
			glVertexAttribPointer(i, attributes[i].dimensions, GL_FLOAT, GL_FALSE, attributes[i].strideBytes, (void*)attributes[i].offsetBytes);
			glEnableVertexAttribArray(i);
		}
		glBindVertexArray(0);
		return buffers;
	}

	void deleteBuffers(const GLBuffers& buffers)
	{
		glDeleteVertexArrays(1, &buffers.vertexArrayObject);
		glDeleteBuffers(1, &buffers.vertexBufferObject);
		glDeleteBuffers(1, &buffers.triangleBuffer);
	}

	unsigned bindTexture(const Texture2D& texture)
	{
		unsigned id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture.attributes.textureWrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture.attributes.textureWrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture.attributes.minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture.attributes.magFilter);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
			texture.width,
			texture.height,
			0,
			texture.attributes.format,
			GL_UNSIGNED_BYTE,
			texture.pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
		return id;
	}

	void framebufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	GLFWwindow* createWindow()
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		GLFWwindow* window = glfwCreateWindow(800, 600, "Game", nullptr, nullptr);
		if (window == nullptr)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return nullptr;
		}
		glfwMakeContextCurrent(window);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return nullptr;
		}
		glViewport(0, 0, 800, 600);
		glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
		return window;
	}

	//void drawSprite(const Texture2D& texture, )
}