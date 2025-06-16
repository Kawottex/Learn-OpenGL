#include "LightScene.h"
#include <Model.h>
#include "VertexArrayInitializer.h"

LightScene::LightScene() : 
	m_litShader(".\\shaders\\litShader.vs", ".\\shaders\\litShader.fs"),
	m_lightSourceShader(".\\shaders\\lightSourceShader.vs", ".\\shaders\\lightSourceShader.fs")
{
	m_diffuseMap = Model::TextureFromFile("container2.png", ".\\resources\\textures");
	m_specularMap = Model::TextureFromFile("container2_specular.png", ".\\resources\\textures");
}

void LightScene::Setup()
{
	SetupMaterial(m_litShader);
	SetupDirectionalLight(m_litShader);
	SetupPointLights(m_litShader);

	VertexArrayInitializer::SetupCube(m_cubeVAO);
	VertexArrayInitializer::SetupCube(m_sourceVAO);

	m_cubePositions = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	m_sourceLightPositions = {
		glm::vec3(0.7f, 0.2f, 2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f, 2.0f, -12.0f),
		glm::vec3(0.0f, 0.0f, -3.0f)
	};

	glEnable(GL_DEPTH_TEST);
}

void LightScene::Draw(const Camera& camera)
{
	m_litShader.Use();
	m_litShader.SetVec3("viewPos", camera.Position);
	UpdateSpotLight(m_litShader, camera);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_specularMap);
	DrawLitCubes(m_cubeVAO, camera, m_litShader);
	
	m_lightSourceShader.Use();
	m_lightSourceShader.SetVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
	DrawSourceLightCubes(m_sourceVAO, camera, m_lightSourceShader);
}

void LightScene::SetupMaterial(Shader& shader)
{
	shader.Use();
	shader.SetInt("material.diffuse", 0);
	shader.SetInt("material.specular", 1);
	shader.SetFloat("material.shininess", 0.25f * 128.0f);
}

void LightScene::SetupDirectionalLight(Shader& shader)
{
	shader.Use();
	shader.SetVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	shader.SetVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	shader.SetVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
	shader.SetVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
}

void LightScene::SetupPointLights(Shader& shader)
{
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f, 0.2f, 2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f, 2.0f, -12.0f),
		glm::vec3(0.0f, 0.0f, -3.0f)
	};

	shader.Use();
	for (int i = 0; i < 4; i++)
	{
		std::string indexStr = std::to_string(i);
		shader.SetVec3("pointLights[" + indexStr + "].position", pointLightPositions[i]);

		shader.SetVec3("pointLights[" + indexStr + "].ambient", glm::vec3(0.1f, 0.1f, 0.1f));
		shader.SetVec3("pointLights[" + indexStr + "].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		shader.SetVec3("pointLights[" + indexStr + "].specular", glm::vec3(1.0f, 1.0f, 1.0f));

		shader.SetFloat("pointLights[" + indexStr + "].constant", 1.0f);
		shader.SetFloat("pointLights[" + indexStr + "].linear", 0.09f);
		shader.SetFloat("pointLights[" + indexStr + "].quadratic", 0.032f);
	}
}

void LightScene::UpdateSpotLight(Shader& shader, const Camera& camera)
{
	shader.Use();
	shader.SetVec3("spotLight.position", camera.Position);
	shader.SetVec3("spotLight.direction", camera.Front);

	shader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	shader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

	shader.SetVec3("spotLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	shader.SetVec3("spotLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	shader.SetVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
}

void LightScene::DrawLitCubes(unsigned int cubeVAO, const Camera& camera, Shader& shader)
{
	glBindVertexArray(cubeVAO);

	int i = 0;
	for (const glm::vec3& cubePos : m_cubePositions)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePos);

		float angle = glm::radians(20.0f * i);
		model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3, 0.5f));
		shader.SetMVPMatrix(model, camera.GetViewMatrix(), camera.GetPerspectiveProj());
		glDrawArrays(GL_TRIANGLES, 0, 36);
		i++;
	}
}

void LightScene::DrawSourceLightCubes(unsigned int VAO, const Camera& camera, Shader& shader)
{
	shader.Use();

	for (const glm::vec3& sourcePos : m_sourceLightPositions)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, sourcePos);
		model = glm::scale(model, glm::vec3(0.2f));

		shader.SetMVPMatrix(model, camera.GetViewMatrix(), camera.GetPerspectiveProj());
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}