#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec3 aColor;\n"
	"out vec3 ourColor;\n"
	"void main()\n"
	"{\n"
	" gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	" ourColor = aColor;\n"
	"}\0";

const char* baseFragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"in vec3 ourColor;\n"
	"void main()\n"
	"{\n"
	" FragColor = vec4(ourColor, 1.0);\n"
	"}\n";

const char* yellowFragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	" FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);"
	"}\n";

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

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

int compileShader(const char* shaderSource, GLenum shaderType, unsigned int& outShader)
{
	unsigned int shader;
	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);

	outShader = shader;

	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
		return -1;
	}
	return 0;
}

int attachShaders(unsigned int vertexShader, unsigned int fragmentShader, unsigned int& outShaderProgram)
{
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	outShaderProgram = shaderProgram;

	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::ATTACH_FAILED\n" << infoLog << std::endl;
		return -1;
	}
	return 0;
}

int setupShaderProgram(const char* fragmentShaderSource, unsigned int& shaderProgram)
{
	unsigned int vertexShader;
	unsigned int fragmentShader;
	compileShader(vertexShaderSource, GL_VERTEX_SHADER, vertexShader);
	compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER, fragmentShader);

	attachShaders(vertexShader, fragmentShader, shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return 0;
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

int setupRectangleVAO(unsigned int& VAO)
{
	float vertices[] = {
		0.5f, 0.5f, 0.0f,	// top right
		0.5f, -0.5f, 0.0f,	// bottom right
		-0.5f, -0.5f, 0.0f,	// bottom left
		-0.5f, 0.5f, 0.0f	// top left
	};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3, // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	setupVBO(vertices, sizeof(vertices));
	setupEBO(indices, sizeof(indices));
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	return 0;
}

void drawTriangle(unsigned int shaderProgram, unsigned int VAO)
{
	int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");

	glUseProgram(shaderProgram);
	//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void drawRectangle(unsigned int shaderProgram, unsigned int VAO)
{
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void mainLoop(GLFWwindow* window)
{
	unsigned int baseShaderProgram;
	unsigned int yellowShaderProgram;
	unsigned int firstVAO;
	unsigned int secVAO;

	setupShaderProgram(baseFragmentShaderSource, baseShaderProgram);
	setupShaderProgram(yellowFragmentShaderSource, yellowShaderProgram);


	float first_tri_vert[] = {
		// positions		// colors
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	float sec_tri_vert[] = {
		0.0f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.25f, 0.5f, 0.0f,
	};

	setupTriangleVAO(firstVAO, first_tri_vert, sizeof(first_tri_vert));
	//setupTriangleVAO(secVAO, sec_tri_vert, sizeof(sec_tri_vert));
	//setupRectangleVAO(VAO);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

		drawTriangle(baseShaderProgram, firstVAO);
		//drawTriangle(baseShaderProgram, secVAO, greenValue);
		//drawRectangle(shaderProgram, VAO);

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