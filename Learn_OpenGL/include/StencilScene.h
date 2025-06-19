#pragma once

#include <Shader.h>
#include <Camera.h>
#include <vector>
#include <ICustomScene.h>

class StencilScene : public ICustomScene
{
public:
	StencilScene();
	virtual void Setup() override;
	virtual void Draw(const Camera& camera) override;

private:
	Shader m_shader;
	Shader m_singleColorShader;

	unsigned int m_cubeVAO = 0;
	unsigned int m_planeVAO = 0;

	unsigned int m_cubeTexture = 0;
	unsigned int m_floorTexture = 0;

	std::vector<float> m_cubeVertices;
	std::vector<float> m_planeVertices;
};