#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>
#include <Camera.h>
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

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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
	//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

	setMVPMatrix(model, shader);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawLightCube(unsigned int VAO, Shader& shader)
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

		float angle = glm::radians(10.0f * i);
		if (i % 3 == 0)
		{
			angle *= (float)glfwGetTime();
		}
		model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3, 0.5f));
		shader.SetMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

unsigned int setupTexture(const char* filename, GLenum texIndex, GLint texFormat)
{
	unsigned int texture;

	glGenTextures(1, &texture);
	glActiveTexture(texIndex);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, texFormat, width, height, 0, texFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	return texture;
}

void updateDeltaTime()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
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

	//setupTexture(".\\resources\\textures\\container.jpg", GL_TEXTURE0, GL_RGB);
	//setupTexture(".\\resources\\textures\\awesomeface.png", GL_TEXTURE1, GL_RGBA);

	Shader shader(".\\shaders\\shader.vs", ".\\shaders\\shader.fs");
	shader.Use();
	shader.SetVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
	shader.SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.SetVec3("lightPos", lightPos);

	Shader lightShader(".\\shaders\\shader.vs", ".\\shaders\\lightShader.fs");

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		updateDeltaTime();
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Use();
		shader.SetVec3("viewPos", camera.Position);
		drawCube(VAO, shader);

		lightShader.Use();
		drawLightCube(lightVAO, lightShader);

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