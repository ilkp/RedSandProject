
#define STB_IMAGE_IMPLEMENTATION

#include <concepts>
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
	EntityComponentSystem<GLBuffers> glBufferSystem(2);
	Mesh2DSystem meshSystem(2);
	ShaderSystem shaderSystem(2);
	RenderUnitSystem renderUnitSystem(5);

	EntityComponentSystem<Mesh2D> meshSystemB(2);

	RenderSystems2D renderSystems;
	renderSystems.glBufferSystem = &glBufferSystem;
	renderSystems.meshSystem = &meshSystem;
	renderSystems.shaderSystem = &shaderSystem;
	renderSystems.textureSystem = &textureSystem;


	// COMPILE SHADER
	Entity shader_entity = entityManager.make();
	Shader* shader = shaderSystem.reserve(shader_entity);
	shader->compile("shader.vert", "shader.frag");
	shader->setInteger("texture1", 0);
	shader->setInteger("texture2", 1);


	// MESH AND VERTEX BUFFERS
	std::vector<float> vertices = {
		//position		//texture coords
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
		mesh->vertices, mesh->verticesSize,
		mesh->triangles, mesh->trianglesSize));


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


	Entity renderUnit_entity = entityManager.make();
	RenderUnit* renderUnit = renderUnitSystem.reserve(renderUnit_entity);
	renderUnit->glBuffer = glBuffer;
	renderUnit->shader = shader_entity;
	renderUnit->textureBinds.push_back({ GL_TEXTURE0, tex1_entity });
	renderUnit->textureBinds.push_back({ GL_TEXTURE1, tex2_entity });
	renderUnit->vertices.insert(mesh_entity);


	// MAIN LOOP
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		Render::draw(renderSystems, renderUnitSystem.get(renderUnit_entity));

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