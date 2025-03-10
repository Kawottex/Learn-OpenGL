#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>
#include <stb_image.h>

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
	return window;
}

void processInput(GLFWwindow* window, float& mixPercentage)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixPercentage += 0.01f;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixPercentage -= 0.01f;
	}
}

unsigned int setupVBO(const float* vertices, unsigned int size)
{
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	return VBO;
}

unsigned int setupEBO(const unsigned int* indices, unsigned int size)
{
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
	return EBO;
}

int setupTriangleVAO(unsigned int& VAO, float* vertices, unsigned int ver_size)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	setupVBO(vertices, ver_size);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	return 0;
}

int setupRectangleVAO(unsigned int& VAO, float* vertices, unsigned int ver_size)
{
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3, // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	setupVBO(vertices, ver_size);
	setupEBO(indices, sizeof(indices));

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	return 0;
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

void setupTriangle(unsigned int& VAO)
{
	float vertices[] = {
		// positions		// colors
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	setupTriangleVAO(VAO, vertices, sizeof(vertices));
}

void setupRectangle(unsigned int& VAO)
{
	float vertices[] = {
		// positions		// colors			// texture coords
		 0.5f,  0.5f, 0.0f,	1.0f, 0.0f, 0.0f, 1.0f, 1.0f,	// top right
		 0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// bottom right
		-0.5f, -0.5f, 0.0f,	0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// bottom left
		-0.5f,  0.5f, 0.0f,	1.0f, 1.0f, 0.0f, 0.0f, 1.0f	// top left
	};

	setupRectangleVAO(VAO, vertices, sizeof(vertices));
}

void mainLoop(GLFWwindow* window)
{
	unsigned int VAO;
	float mixPercentage = 0.0f;

	//setupTriangle(VAO);
	setupRectangle(VAO);
	setupTexture(".\\resources\\textures\\container.jpg", GL_TEXTURE0, GL_RGB);
	setupTexture(".\\resources\\textures\\awesomeface.png", GL_TEXTURE1, GL_RGBA);

	Shader shader(".\\shaders\\shader.vs", ".\\shaders\\shader.fs");
	shader.Use();
	shader.SetInt("texture1", 0);
	shader.SetInt("texture2", 1);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window, mixPercentage);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//double timeValue = glfwGetTime();

		shader.Use();
		shader.SetFloat("mixPercentage", mixPercentage);
		//drawTriangle(VAO);
		drawRectangle(VAO);

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