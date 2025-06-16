#pragma once

#include "Shader.h"
#include "Camera.h"
#include <vector>

class LightScene
{
public:
	LightScene();
	void Setup();
	void Draw(const Camera& camera);

private:
	void SetupMaterial(Shader& shader);
	void SetupDirectionalLight(Shader& shader);
	void SetupPointLights(Shader& shader);
	void UpdateSpotLight(Shader& shader, const Camera& camera);

	void DrawLitCubes(unsigned int cubeVAO, const Camera& camera, Shader& shader);
	void DrawSourceLightCubes(unsigned int VAO, const Camera& camera, Shader& shader);

private:
	Shader m_litShader;
	Shader m_lightSourceShader;
	unsigned int m_diffuseMap = 0;
	unsigned int m_specularMap = 0;

	unsigned int m_cubeVAO = 0;
	unsigned int m_sourceVAO = 0;
	std::vector<glm::vec3> m_cubePositions;
	std::vector<glm::vec3> m_sourceLightPositions;
};