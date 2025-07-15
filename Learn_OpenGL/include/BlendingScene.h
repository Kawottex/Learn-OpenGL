#pragma once

#include <Shader.h>
#include <Camera.h>
#include <vector>
#include <ICustomScene.h>

class BlendingScene : public ICustomScene
{
public:
	BlendingScene();
	virtual void Setup() override;
	virtual void Draw(const Camera& camera) override;

private:
	Shader m_shader;

	unsigned int m_cubeVAO = 0;
	unsigned int m_planeVAO = 0;
	unsigned int m_transparentVAO = 0;

	unsigned int m_cubeTexture = 0;
	unsigned int m_floorTexture = 0;
	unsigned int m_transparentTexture = 0;

	std::vector<glm::vec3> windowsPos;
};