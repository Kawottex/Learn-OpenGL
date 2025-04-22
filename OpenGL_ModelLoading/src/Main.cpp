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

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f, 0.2f, 2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f, 2.0f, -12.0f),
		glm::vec3(0.0f, 0.0f, -3.0f)
	};

	Shader modelShader(".\\shaders\\shader.vs", ".\\shaders\\shader.fs");
	modelShader.Use();
	modelShader.SetFloat("material.shininess", 0.25f * 128.0f);

	setupDirectionalLight(modelShader);
	setupPointLights(modelShader, pointLightPositions);

	stbi_set_flip_vertically_on_load(true);
	glEnable(GL_DEPTH_TEST);

	std::string backpackPath = ".\\resources\\models\\backpack\\backpack.obj";
	Model backpackModel(backpackPath.c_str());

	while (!glfwWindowShouldClose(window))
	{
		updateDeltaTime();
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		modelShader.Use();
		updateSpotLight(modelShader);
		modelShader.SetVec3("viewPos", camera.Position);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		setMVPMatrix(model, modelShader);

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