#pragma once

#include <Shader.h>
#include <Camera.h>
#include <Model.h>
#include <vector>
#include <ICustomScene.h>

class ModelScene : public ICustomScene
{
public:
	ModelScene();
	virtual void Setup() override;
	virtual void Draw(const Camera& camera) override;

private:
	void SetupMaterial(Shader& shader);
	void SetupDirectionalLight(Shader& shader);
	void SetupPointLights(Shader& shader);
	void UpdateSpotLight(Shader& shader, const Camera& camera);

	Shader m_modelShader;
	Model m_model;
	std::vector<glm::vec3> m_sourceLightPositions;
};