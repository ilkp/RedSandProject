
#define STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "core.h"
#include "Shader.h"
#include "Renderer.h"
#include "stb_image.h"

void processInput(GLFWwindow* window);

int main()
{
	glfwInit();
	GLFWwindow* window = Render::createWindow();
	if (window == nullptr)
		return -1;

	EntityManager entityManager;
	Texture2DSystem textureSystem(10);
	GLBufferSystem glBufferSystem(2);
	Mesh2DSystem meshSystem(2);

	Shader shaderA("shader.vert", "shader.frag");
	Shader shaderB("color_shader.vert", "color_shader.frag");


	// MESH AND VERTEX BUFFERS
	std::vector<float> vertices = {
		-0.5f,  0.5f,	-1.0f,  1.0f,
		 0.5f,  0.5f,	 1.0f,  1.0f,
		 0.5f, -0.5f,	 1.0f, -1.0f,
		-0.5f, -0.5f,	-1.0f, -1.0f
	};
	std::vector<unsigned int> triangles = {
		0, 1, 2,
		2, 3, 0
	};

	Entity mesh_entity = entityManager.make();
	meshSystem.reserve(mesh_entity);
	meshSystem.copy(mesh_entity, vertices.data(), vertices.size(), triangles.data(), triangles.size());
	meshSystem.calculateBounds(mesh_entity, 4);

	Entity glBuffer = entityManager.make();
	std::vector<VertexAttributes> vertexAttrb;
	vertexAttrb.push_back(VertexAttributes(2, 4 * sizeof(float), 0));
	vertexAttrb.push_back(VertexAttributes(2, 4 * sizeof(float), 2 * sizeof(float)));
	glBufferSystem.reserve(glBuffer);
	Mesh2D* mesh = meshSystem.get(mesh_entity);
	glBufferSystem.set(glBuffer, Render::bindBuffers(
		vertexAttrb,
		mesh->vertices, mesh->verticesLength,
		mesh->triangles, mesh->trianglesLength));


	// TEXTURES
	Entity tex1_entity = entityManager.make();
	Texture2D* texture1 = textureSystem.reserve(tex1_entity);
	texture1->attributes = Texture2DAttributes(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_RGB);
	texture1->pixels = stbi_load("C:/Projects/RedSandProject/resources/container.jpg", &texture1->width, &texture1->height, &texture1->nChannels, 0);
	textureSystem.set(tex1_entity, Render::bindTexture(*texture1));

	Entity tex2_entity = entityManager.make();
	Texture2D* texture2 = textureSystem.reserve(tex2_entity);
	texture2->attributes = Texture2DAttributes(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_RGBA);
	texture2->pixels = stbi_load("C:/Projects/RedSandProject/resources/awesomeface.png", &texture2->width, &texture2->height, &texture2->nChannels, 0);
	textureSystem.set(tex2_entity, Render::bindTexture(*texture2));

	shaderA.use();
	shaderA.setInteger("texture1", 0);
	shaderA.setInteger("texture2", 1);


	// MAIN LOOP
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureSystem.getId(tex1_entity));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureSystem.getId(tex2_entity));

		glm::mat4 trans = glm::mat4(1.0f);
		//trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));

		shaderA.use();
		unsigned int transformLoc = glGetUniformLocation(shaderA.id, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		glBindVertexArray(glBufferSystem.get(glBuffer).vertexArrayObject);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// CLEAN
	glBufferSystem.release(glBuffer);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}