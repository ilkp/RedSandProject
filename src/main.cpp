
#define STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Shader.h"
#include "Renderer.h"
#include "core.h"
#include "stb_image.h"

void processInput(GLFWwindow* window);

int main()
{
	glfwInit();
	GLFWwindow* window = createWindow();
	if (window == nullptr)
		return -1;

	EntityManager entityManager;
	Texture2DSystem textureSystem(10);
	GLBufferSystem glBufferSystem(2);

	Shader shaderA("shader.vert", "shader.frag");
	Shader shaderB("color_shader.vert", "color_shader.frag");


	// MESH AND VERTEX BUFFERS
	std::vector<float> vertices {
		// position				// texture coords
		 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f
	};
	std::vector<unsigned int> triangles {
		0, 1, 3,
		1, 2, 3
	};

	Entity glBuffer = entityManager.make();
	std::vector<VertexAttributes> vertexAttrb = {
		VertexAttributes { 3, 5 * sizeof(float), 0 },
		VertexAttributes { 2, 5 * sizeof(float), 3 * sizeof(float) }
	};
	glBufferSystem.reserve(glBuffer);
	glBufferSystem.load(glBuffer, vertexAttrb, vertices, triangles);


	// TEXTURES
	Texture2D texture1, texture2;
	Entity tex1_entity = entityManager.make();
	textureSystem.reserve(tex1_entity);
	texture1.attributes = Texture2DAttributes(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_RGB);
	texture1.pixels = stbi_load("C:/Projects/RedSandProject/resources/container.jpg", &texture1.width, &texture1.height, &texture1.nChannels, 0);
	textureSystem.set(tex1_entity, texture1);
	textureSystem.load(tex1_entity);

	Entity tex2_entity = entityManager.make();
	textureSystem.reserve(tex2_entity);
	texture2.attributes = Texture2DAttributes(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_RGBA);
	texture2.pixels = stbi_load("C:/Projects/RedSandProject/resources/awesomeface.png", &texture2.width, &texture2.height, &texture2.nChannels, 0);
	textureSystem.set(tex2_entity, texture2);
	textureSystem.load(tex2_entity);

	shaderA.use();
	shaderA.setInt("texture1", 0);
	shaderA.setInt("texture2", 1);


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
		trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));

		shaderA.use();
		unsigned int transformLoc = glGetUniformLocation(shaderA.id, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		glBindVertexArray(glBufferSystem.get(glBuffer).value().vertArrayObject);
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