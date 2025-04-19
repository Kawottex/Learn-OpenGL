#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>
#include <Camera.h>
#include <Model.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <VertexArrayInitializer.h>

Camera camera;

float lastX = 400, lastY = 300;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool xDirection = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void initGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = xpos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed: y ranges bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

GLFWwindow* initGLFWWindow()
{
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	return window;
}

void processInput(GLFWwindow* window)
{
	const float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	}
}

void setMVPMatrix(const glm::mat4& model, Shader& shader)
{
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.f);

	shader.SetMat4("model", model);
	shader.SetMat4("view", view);
	shader.SetMat4("projection", projection);
}

void drawTriangle(unsigned int VAO)
{
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void drawRectangle(unsigned int VAO)
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void drawCube(unsigned int VAO, Shader& shader)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

	setMVPMatrix(model, shader);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawLightCube(unsigned int VAO, Shader& shader, glm::vec3 lightPos)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f));

	setMVPMatrix(model, shader);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawCubeArray(unsigned int VAO, Shader& shader, glm::vec3 cubePositions[])
{
	glBindVertexArray(VAO);

	glm::mat4 view = camera.GetViewMatrix();
	shader.SetMat4("view", view);

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.f);
	shader.SetMat4("projection", projection);

	for (int i = 0; i < 10; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);

		float angle = glm::radians(20.0f * i);
		model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3, 0.5f));
		shader.SetMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

unsigned int loadTexture(const char* filename)
{
	unsigned int texture;

	glGenTextures(1, &texture);

	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

	if (data)
	{
		GLenum format = GL_RGBA;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	return texture;
}

void updateDeltaTime()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void setupMaterial(Shader& shader)
{
	shader.Use();
	shader.SetInt("material.diffuse", 0);
	shader.SetInt("material.specular", 1);
	shader.SetFloat("material.shininess", 0.25f * 128.0f);
}

void setupDirectionalLight(Shader& shader)
{
	shader.Use();
	shader.SetVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	shader.SetVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	shader.SetVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
	shader.SetVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
}

void setupPointLights(Shader& shader, glm::vec3* pointLightPositions)
{
	shader.Use();
	for (int i = 0; i < 4; i++)
	{
		std::string indexStr = std::to_string(i);
		shader.SetVec3("pointLights[" + indexStr + "].position", pointLightPositions[i]);
	
		shader.SetVec3("pointLights[" + indexStr + "].ambient", glm::vec3(0.1f, 0.1f, 0.1f));
		shader.SetVec3("pointLights[" + indexStr + "].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		shader.SetVec3("pointLights[" + indexStr + "].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	
		shader.SetFloat("pointLights[" + indexStr + "].constant", 1.0f);
		shader.SetFloat("pointLights[" + indexStr + "].linear", 0.09f);
		shader.SetFloat("pointLights[" + indexStr + "].quadratic", 0.032f);
	}
}

void updateSpotLight(Shader& shader)
{
	shader.Use();
	shader.SetVec3("spotLight.position", camera.Position);
	shader.SetVec3("spotLight.direction", camera.Front);

	shader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	shader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

	shader.SetVec3("spotLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	shader.SetVec3("spotLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	shader.SetVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
}

void mainLoop(GLFWwindow* window)
{
	unsigned int VAO;
	unsigned int lightVAO;
	float mixPercentage = 0.2f;

	camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	VertexArrayInitializer vaInit;
	vaInit.SetupCube(VAO);
	vaInit.SetupCube(lightVAO);

	unsigned int diffuseMap = loadTexture(".\\resources\\textures\\container2.png");
	unsigned int specularMap = loadTexture(".\\resources\\textures\\container2_specular.png");

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f, 0.2f, 2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f, 2.0f, -12.0f),
		glm::vec3(0.0f, 0.0f, -3.0f)
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	Shader lightSourceShader(".\\shaders\\shader.vs", ".\\shaders\\lightShader.fs");
	Shader cubeShader(".\\shaders\\shader.vs", ".\\shaders\\shader.fs");
	Shader modelShader(".\\shaders\\shader.vs", ".\\shaders\\model_loading.fs");

	setupMaterial(cubeShader);
	setupDirectionalLight(cubeShader);
	setupPointLights(cubeShader, pointLightPositions);

	glEnable(GL_DEPTH_TEST);

	std::string backpackPath = ".\\resources\\models\\backpack\\backpack.obj";
	Model backpackModel(backpackPath.c_str());

	while (!glfwWindowShouldClose(window))
	{
		updateDeltaTime();
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cubeShader.Use();
		cubeShader.SetVec3("viewPos", camera.Position);
		updateSpotLight(cubeShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		drawCubeArray(VAO, cubeShader, cubePositions);

		lightSourceShader.Use();
		lightSourceShader.SetVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
		for (int i = 0; i < 4; i++)
		{
			drawLightCube(lightVAO, lightSourceShader, pointLightPositions[i]);
		}

		backpackModel.Draw(modelShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

int main()
{
	initGLFW();
	GLFWwindow* window = initGLFWWindow();
	if (!window)
	{
		return -1;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return NULL;
	}

	mainLoop(window);

	glfwTerminate();
	return 0;
}