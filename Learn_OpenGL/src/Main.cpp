#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>
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

glm::mat4 GetPerspectiveProj()
{
	return glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.f);
}

glm::mat4 GetInvertedView()
{
	camera.Front *= -1;
	glm::mat4 view = camera.GetViewMatrix();
	camera.Front *= -1;
	return view;
}

glm::mat4 GetViewNoTranslate()
{
	glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	return view;
}

void setMVPMatrix(Shader& shader, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
	shader.Use();
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

void setupVertex(float* vertices, int vertSize, int posCount, int texCount, unsigned int& VAO, unsigned int& VBO)
{
	int posTexCount = posCount + texCount;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, posCount, GL_FLOAT, GL_FALSE, posTexCount * sizeof(float), (void*)0);

	if (texCount > 0)
	{
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, texCount, GL_FLOAT, GL_FALSE, posTexCount * sizeof(float), (void*)(posCount * sizeof(float)));
	}

	glBindVertexArray(0);
}

void setupMirrorQuad(unsigned int& VAO, unsigned int& VBO)
{
	float quadVertices[] = {
		// positions // texCoords
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f, 1.0f
	};

	setupVertex(quadVertices, sizeof(quadVertices), 2, 2, VAO, VBO);
}

void setupScreenQuad(unsigned int& VAO, unsigned int& VBO)
{
	float quadVertices[] = {
		// positions // texCoords
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	setupVertex(quadVertices, sizeof(quadVertices), 2, 2, VAO, VBO);
}

void setupQuad(unsigned int& VAO, unsigned int& VBO)
{
	float vertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};
	setupVertex(vertices, sizeof(vertices), 3, 2, VAO, VBO);
}

void setupCube(unsigned int& VAO, unsigned int& VBO)
{
	float cubeVertices[] = {
		// back face
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		// front face
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, // top-left
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		// left face
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
		// right face
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		// bottom face
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, // top-left
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
		// top face
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f // bottom-left
	};

	setupVertex(cubeVertices, sizeof(cubeVertices), 3, 2, VAO, VBO);
}

void setupCubeNoTexture(unsigned int& VAO, unsigned int& VBO)
{
	float cubeVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	setupVertex(cubeVertices, sizeof(cubeVertices), 3, 0, VAO, VBO);
}

void setupPlane(unsigned int& VAO, unsigned int& VBO)
{
	float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};

	setupVertex(planeVertices, sizeof(planeVertices), 3, 2, VAO, VBO);
}

void drawFloor(Shader& shader, unsigned int planeVAO, unsigned int floorTexture)
{
	glStencilMask(0x00);

	shader.Use();
	glBindVertexArray(planeVAO);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	shader.SetMat4("model", glm::mat4(1.0f));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void drawCubes(Shader& shader, unsigned int cubeVAO, unsigned int cubeTexture)
{
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	shader.Use();
	glm::mat4 model = glm::mat4(1.0f);
	glBindVertexArray(cubeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
	shader.SetMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
	shader.SetMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawCubesBorder(Shader& shader, unsigned int cubeVAO)
{
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);

	shader.Use();
	glm::mat4 model = glm::mat4(1.0f);
	glBindVertexArray(cubeVAO);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
	model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
	shader.SetMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
	shader.SetMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glEnable(GL_DEPTH_TEST);
}

void drawQuadArray(Shader& shader, unsigned int quadVAO, unsigned int texture, const vector<glm::vec3>& quadArray)
{
	std::map<float, glm::vec3> sorted;
	for (unsigned int i = 0; i < quadArray.size(); i++)
	{
		float distance = glm::length(camera.Position - quadArray[i]);
		sorted[distance] = quadArray[i];
	}

	shader.Use();

	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, texture);

	glm::mat4 model = glm::mat4(1.0f);
	for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it !=
		sorted.rend(); ++it)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, it->second);
		shader.SetMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void drawScreenQuad(Shader& shader, unsigned int quadVAO, unsigned int texture)
{
	shader.Use();
	glBindVertexArray(quadVAO);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void setupFramebuffer(unsigned int &fbo, unsigned int& texColorBuffer)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// texture setup
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// apply texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	// render buffer
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int loadCubemap(const vector<string>& faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	unsigned char* data;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width,
				height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Cubemap failed to load at path: " << faces[i] << std::endl;
		}
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	return textureID;
}

void rearViewLoop(GLFWwindow* window)
{
	camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	unsigned int cubeVAO, cubeVBO, planeVAO, planeVBO, quadVAO, quadVBO;
	unsigned int screenQuadVAO, screenQuadVBO, mirrorQuadVAO, mirrorQuadVBO;

	setupCube(cubeVAO, cubeVBO);
	setupPlane(planeVAO, planeVBO);
	setupQuad(quadVAO, quadVBO);
	setupScreenQuad(screenQuadVAO, screenQuadVBO);
	setupMirrorQuad(mirrorQuadVAO, mirrorQuadVBO);

	Shader shader(".\\shaders\\depth_testing.vs", ".\\shaders\\depth_testing.fs");
	Shader borderShader(".\\shaders\\depth_testing.vs", ".\\shaders\\shaderSingleColor.fs");
	Shader screenShader(".\\shaders\\screenShader.vs", ".\\shaders\\screenShader.fs");

	unsigned int cubeTexture = Model::TextureFromFile("marble.jpg", ".\\resources\\textures");
	unsigned int floorTexture = Model::TextureFromFile("metal.png", ".\\resources\\textures");
	unsigned int windowTexture = Model::TextureFromFile("blending_transparent_window.png", ".\\resources\\textures");

	stbi_set_flip_vertically_on_load(true);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	vector<glm::vec3> quadArrayPos;
	quadArrayPos.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
	quadArrayPos.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
	quadArrayPos.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
	quadArrayPos.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	quadArrayPos.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

	unsigned int framebuffer, texColorBuffer;
	unsigned int mirrorBuffer, texMirrorBuffer;

	setupFramebuffer(framebuffer, texColorBuffer);
	setupFramebuffer(mirrorBuffer, texMirrorBuffer);

	while (!glfwWindowShouldClose(window))
	{
		updateDeltaTime();
		processInput(window);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Setup scene framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// Draw scene in framebuffer
		glm::mat4 model = glm::mat4(1.0f);
		setMVPMatrix(shader, model, camera.GetViewMatrix(), GetPerspectiveProj());
		setMVPMatrix(borderShader, model, camera.GetViewMatrix(), GetPerspectiveProj());
		drawFloor(shader, planeVAO, floorTexture);
		drawCubes(shader, cubeVAO, cubeTexture);
		drawQuadArray(shader, quadVAO, windowTexture, quadArrayPos);

		// Setup mirror framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, mirrorBuffer);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// Draw mirrored scene in framebuffer
		model = glm::mat4(1.0f);
		setMVPMatrix(shader, model, GetInvertedView(), GetPerspectiveProj());
		drawFloor(shader, planeVAO, floorTexture);
		drawCubes(shader, cubeVAO, cubeTexture);
		drawQuadArray(shader, quadVAO, windowTexture, quadArrayPos);

		// Draw screen quads
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		drawScreenQuad(screenShader, screenQuadVAO, texColorBuffer);
		drawScreenQuad(screenShader, mirrorQuadVAO, texMirrorBuffer);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &planeVBO);
}

vector<std::string> getSkyboxFaces()
{
	vector<std::string> faces
	{
		".\\resources\\textures\\skybox\\right.jpg",
		".\\resources\\textures\\skybox\\left.jpg",
		".\\resources\\textures\\skybox\\top.jpg",
		".\\resources\\textures\\skybox\\bottom.jpg",
		".\\resources\\textures\\skybox\\front.jpg",
		".\\resources\\textures\\skybox\\back.jpg"
	};
	return faces;
}

vector<glm::vec3> getQuadArrayPos()
{
	vector<glm::vec3> quadArrayPos
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};
	return quadArrayPos;
}

void drawSkybox(Shader& shader, unsigned int skyboxVAO, unsigned int cubemapTexture)
{
	glDepthFunc(GL_LEQUAL);
	shader.Use();
	glm::mat4 model = glm::mat4(1.0f);
	setMVPMatrix(shader, model, GetViewNoTranslate(), GetPerspectiveProj());
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_LESS);
}

void mainLoop(GLFWwindow* window)
{
	bool bEnableFramebuffer = false;

	camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	unsigned int cubeVAO, cubeVBO, planeVAO, planeVBO, quadVAO, quadVBO;
	unsigned int screenQuadVAO, screenQuadVBO, skyboxVAO, skyboxVBO;

	setupCube(cubeVAO, cubeVBO);
	setupPlane(planeVAO, planeVBO);
	setupQuad(quadVAO, quadVBO);
	setupScreenQuad(screenQuadVAO, screenQuadVBO);
	setupCubeNoTexture(skyboxVAO, skyboxVBO);

	Shader shader(".\\shaders\\depth_testing.vs", ".\\shaders\\depth_testing.fs");
	Shader borderShader(".\\shaders\\depth_testing.vs", ".\\shaders\\shaderSingleColor.fs");
	Shader screenShader(".\\shaders\\screenShader.vs", ".\\shaders\\screenShader.fs");
	Shader skyboxShader(".\\shaders\\skyboxShader.vs", ".\\shaders\\skyboxShader.fs");

	unsigned int cubeTexture = Model::TextureFromFile("marble.jpg", ".\\resources\\textures");
	unsigned int floorTexture = Model::TextureFromFile("metal.png", ".\\resources\\textures");
	unsigned int vegetationTexture = Model::TextureFromFile("grass.png", ".\\resources\\textures");
	unsigned int windowTexture = Model::TextureFromFile("blending_transparent_window.png", ".\\resources\\textures");

	vector<std::string> faces = getSkyboxFaces();
	unsigned int cubemapTexture = loadCubemap(faces);

	stbi_set_flip_vertically_on_load(true);
	glEnable(GL_DEPTH_TEST);
	//
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	vector<glm::vec3> quadArrayPos = getQuadArrayPos();

	unsigned int framebuffer, texColorBuffer;
	setupFramebuffer(framebuffer, texColorBuffer);

	while (!glfwWindowShouldClose(window))
	{
		updateDeltaTime();
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (bEnableFramebuffer)
		{
			// Setup scene framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glEnable(GL_DEPTH_TEST);
		}

		// Draw scene
		glm::mat4 model = glm::mat4(1.0f);
		setMVPMatrix(shader, model, camera.GetViewMatrix(), GetPerspectiveProj());
		setMVPMatrix(borderShader, model, camera.GetViewMatrix(), GetPerspectiveProj());
		drawFloor(shader, planeVAO, floorTexture);
		drawCubes(shader, cubeVAO, cubeTexture);
		//drawCubesBorder(borderShader, cubeVAO);
		drawQuadArray(shader, quadVAO, windowTexture, quadArrayPos);

		if (bEnableFramebuffer)
		{
			// Draw screen quads
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			drawScreenQuad(screenShader, screenQuadVAO, texColorBuffer);
		}

		// Skybox
		drawSkybox(skyboxShader, skyboxVAO, cubemapTexture);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &planeVBO);
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