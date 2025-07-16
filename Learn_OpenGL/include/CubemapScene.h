#pragma once

#include <Shader.h>
#include <Camera.h>
#include <vector>
#include <ICustomScene.h>

class CubemapScene : public ICustomScene
{
public:
	CubemapScene();
	virtual void Setup() override;
	virtual void Draw(const Camera& camera) override;

private:
	void SetupFramebuffer();
	unsigned int LoadCubemap(const std::vector<std::string>& faces);
	void DrawScreenQuad(Shader& shader, unsigned int quadVAO, unsigned int texture);
	void DrawCubes(Shader& shader, unsigned int cubeVAO, unsigned int cubeTexture);
	void DrawSkybox(Shader& shader, unsigned int skyboxVAO, unsigned int cubemapTexture, const Camera& camera);

private:
	Shader m_shader;
	Shader m_screenShader;
	Shader m_borderShader;
	Shader m_skyboxShader;

	unsigned int m_cubeVAO = 0;
	unsigned int m_planeVAO = 0;
	unsigned int m_quadVAO = 0;
	unsigned int m_screenQuadVAO = 0;
	unsigned int m_skyboxVAO = 0;
	unsigned int m_cubeReflectionVAO = 0;

	unsigned int m_cubeTexture = 0;
	unsigned int m_cubemapTexture = 0;

	unsigned int m_framebuffer = 0;
	unsigned int m_textureColorbuffer = 0;

	bool m_bEnableFramebuffer;
};