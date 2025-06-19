#include <StencilScene.h>
#include <Model.h>
#include <VertexArrayInitializer.h>
#include <stb_image.h>

StencilScene::StencilScene()
{
}

void StencilScene::Setup()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	m_shader.LoadShader(".\\shaders\\stencil_testing.vs", ".\\shaders\\stencil_testing.fs");
	m_singleColorShader.LoadShader(".\\shaders\\stencil_testing.vs", ".\\shaders\\stencil_single_color.fs");
	m_cubeTexture = Model::TextureFromFile("marble.jpg", ".\\resources\\textures");
	m_floorTexture = Model::TextureFromFile("metal.png", ".\\resources\\textures");

	VertexArrayInitializer::SetupCubeNoNormal(m_cubeVAO);
	VertexArrayInitializer::SetupPlane(m_planeVAO);

	m_shader.Use();
	m_shader.SetInt("texture1", 0);
}

void StencilScene::Draw(const Camera& camera)
{
	m_singleColorShader.Use();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	m_singleColorShader.SetMVPMatrix(model, camera.GetViewMatrix(), camera.GetPerspectiveProj());

	m_shader.Use();
	m_shader.SetMat4("view", camera.GetViewMatrix());
	m_shader.SetMat4("projection", camera.GetPerspectiveProj());
	
    // draw floor as normal, but don't write the floor to the stencil buffer, we only care about the containers. We set its mask to 0x00 to not write to the stencil buffer.
    glStencilMask(0x00);
    // floor
    glBindVertexArray(m_planeVAO);
    glBindTexture(GL_TEXTURE_2D, m_floorTexture);
    m_shader.SetMat4("model", glm::mat4(1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // 1st. render pass, draw objects as normal, writing to the stencil buffer
    // --------------------------------------------------------------------
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    // cubes
    glBindVertexArray(m_cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_cubeTexture);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    m_shader.SetMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    m_shader.SetMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
    // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
    // the objects' size differences, making it look like borders.
    // -----------------------------------------------------------------------------------------------------------------------------
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    m_singleColorShader.Use();
    float scale = 1.1f;
    // cubes
    glBindVertexArray(m_cubeVAO);
    glBindTexture(GL_TEXTURE_2D, m_cubeTexture);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    m_singleColorShader.SetMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    m_singleColorShader.SetMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);
}