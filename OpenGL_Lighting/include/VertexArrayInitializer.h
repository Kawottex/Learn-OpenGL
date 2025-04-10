#pragma once

class VertexArrayInitializer
{
public:
	void SetupTriangle(unsigned int& VAO);
	void SetupRectangle(unsigned int& VAO);
	void SetupCube(unsigned int& VAO);

private:
	unsigned int setupEBO(const unsigned int* indices, unsigned int size);
	unsigned int setupVBO(const float* vertices, unsigned int size);

	int setupCubeVAO(unsigned int& VAO, float* vertices, unsigned int ver_size);
	int setupRectangleVAO(unsigned int& VAO, float* vertices, unsigned int ver_size);
	int setupTriangleVAO(unsigned int& VAO, float* vertices, unsigned int ver_size);
	int setupLightVAO(unsigned int& VAO, float* vertices, unsigned int ver_size);
};