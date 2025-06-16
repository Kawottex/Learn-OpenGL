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
	Model();
	Model(const char* path);
	void LoadModel(string path);
	void Draw(Shader& shader);

	static unsigned int TextureFromFile(const char* path, const string& directory);

private:
	vector<Texture> textures_loaded;
	vector<Mesh> meshes;
	string directory;

private:
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};