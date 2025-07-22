#include <ModelScene.h>
#include <Model.h>
#include <VertexArrayInitializer.h>
#include <stb_image.h>

ModelScene::ModelScene()
{
}

void ModelScene::Setup()
{
	m_modelShader.LoadShader(".\\shaders\\litShader.vs", ".\\shaders\\litShader.fs");
	SetupMaterial(m_modelShader);
	SetupDirectionalLight(m_modelShader);
	SetupPointLights(m_modelShader);

	m_sourceLightPositions = {
		glm::vec3(0.7f, 0.2f, 2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f, 2.0f, -12.0f),
		glm::vec3(0.0f, 0.0f, -3.0f)
	};

	stbi_set_flip_vertically_on_load(true);
	glEnable(GL_DEPTH_TEST);

	m_model.LoadModel(".\\resources\\models\\backpack\\backpack.blobj");
}

void ModelScene::Draw(const Camera& camera)
{
	m_modelShader.Use();
	UpdateSpotLight(m_modelShader, camera);
	m_modelShader.SetVec3("viewPos", camera.Position);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	m_modelShader.SetMVPMatrix(model, camera.GetViewMatrix(), camera.GetPerspectiveProj());

	m_model.Draw(m_modelShader);
}

void ModelScene::SetupMaterial(Shader& shader)
{
	shader.Use();
	shader.SetFloat("material.shininess", 0.25f * 128.0f);
}

void ModelScene::SetupDirectionalLight(Shader& shader)
{
	shader.Use();
	shader.SetVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	shader.SetVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	shader.SetVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
	shader.SetVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
}

void ModelScene::SetupPointLights(Shader& shader)
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

void ModelScene::UpdateSpotLight(Shader& shader, const Camera& camera)
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
