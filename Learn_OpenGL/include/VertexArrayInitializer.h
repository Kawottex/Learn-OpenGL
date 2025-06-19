#pragma once

class VertexArrayInitializer
{
public:
	static void SetupTriangle(unsigned int& VAO);
	static void SetupRectangle(unsigned int& VAO);
	static void SetupCube(unsigned int& VAO);
	static void SetupCubeNoTexture(unsigned int& VAO);
	static void SetupCubeNoNormal(unsigned int& VAO);
	static void Setup2DQuad(unsigned int& VAO, const float* vertices, unsigned int size);
	static void Setup3DQuad(unsigned int& VAO);
	static void SetupPlane(unsigned int& VAO);
	static void SetupMirrorQuad(unsigned int& VAO);
	static void SetupScreenQuad(unsigned int& VAO);

private:
	static unsigned int setupEBO(const unsigned int* indices, unsigned int size);
	static unsigned int setupVBO(const float* vertices, unsigned int size);
};