#pragma once

#include <glm/glm.hpp>
#include <Shader.h>
#include <string>
#include <vector>

using namespace std;


struct Vertex 
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture 
{
	unsigned int id;
	string type;
};

class Mesh
{
public:
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices,
		vector<Texture> textures);

	void Draw(Shader& shader);

public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

private:
	void setupMesh();

private:
	unsigned int VAO, VBO, EBO;
};