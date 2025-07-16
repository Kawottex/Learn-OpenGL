#pragma once

#include <Shader.h>
#include <Camera.h>
#include <vector>
#include <ICustomScene.h>

class MirrorFramebufferScene : public ICustomScene
{
public:
	MirrorFramebufferScene();
	virtual void Setup() override;
	virtual void Draw(const Camera& camera) override;

private:
	void SetupFramebuffer(unsigned int& fbo, unsigned int& texColorBuffer);
	void DrawQuadArray(Shader& shader, unsigned int quadVAO, unsigned int texture, const std::vector<glm::vec3>& quadArray, const Camera& camera);
	void DrawCubes(Shader& shader, unsigned int cubeVAO, unsigned int cubeTexture);
	void DrawFloor(Shader& shader, unsigned int planeVAO, unsigned int floorTexture);
	void DrawScreenQuad(Shader& shader, unsigned int quadVAO, unsigned int texture);
	glm::mat4 GetInvertedView(const Camera& camera);

private:
	std::vector<glm::vec3> m_quadArrayPos;

	Shader m_shader;
	Shader m_borderShader;
	Shader m_screenShader;

	unsigned int m_cubeVAO = 0;
	unsigned int m_planeVAO = 0;
	unsigned int m_quadVAO = 0;
	unsigned int m_screenQuadVAO = 0;
	unsigned int m_mirrorQuadVAO = 0;

	unsigned int m_cubeTexture = 0;
	unsigned int m_floorTexture = 0;
	unsigned int m_windowTexture = 0;

	unsigned int m_framebuffer = 0;
	unsigned int m_texColorbuffer = 0;

	unsigned int m_mirrorFramebuffer = 0;
	unsigned int m_mirrorTexColorbuffer = 0;
};