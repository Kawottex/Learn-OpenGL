#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	// the program ID
	unsigned int ID;

	// constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath);

	// use/activate the shader
	void Use();

	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;

private:
	std::string GetCodeFromFile(const char* filePath);
	int CompileShader(const char* shaderSource, GLenum shaderType, unsigned int& outShader);
	int SetupProgram(unsigned int vertexShader, unsigned int fragmentShader);
};