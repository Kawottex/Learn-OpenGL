#pragma once

#include <Shader.h>
#include <Camera.h>
#include <vector>
#include <ICustomScene.h>

class FramebufferScene : public ICustomScene
{
public:
	FramebufferScene();
	virtual void Setup() override;
	virtual void Draw(const Camera& camera) override;

private:
	void SetupFramebuffer();

private:
	Shader m_shader;
	Shader m_screenShader;

	unsigned int m_cubeVAO = 0;
	unsigned int m_planeVAO = 0;
	unsigned int m_quadVAO = 0;

	unsigned int m_cubeTexture = 0;
	unsigned int m_floorTexture = 0;

	unsigned int m_framebuffer = 0;
	unsigned int m_textureColorbuffer = 0;
};