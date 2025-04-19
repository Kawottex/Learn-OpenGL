#pragma once

#include <Shader.h>
#include <Mesh.h>
#include <assimp/scene.h>

#include <vector>
#include <string>

using namespace std;

class Model
{
public:
	Model(const char* path);
	void Draw(Shader& shader);

private:
	vector<Texture> textures_loaded;
	vector<Mesh> meshes;
	string directory;

	void loadModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	unsigned int textureFromFile(const char* path, const string& directory);
};