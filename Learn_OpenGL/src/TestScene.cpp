#include "TestScene.h"
#include <VertexArrayInitializer.h>
#include <Model.h>
#include <map>

TestScene::TestScene()
{
}

void TestScene::Setup()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shader.LoadShader(".\\shaders\\testShader.vs", ".\\shaders\\testShader.fs");

    m_cubeTexture = Model::TextureFromFile("marble.jpg", ".\\resources\\textures");
    m_floorTexture = Model::TextureFromFile("metal.png", ".\\resources\\textures");
    m_transparentTexture = Model::TextureFromFile("blending_transparent_window.png", ".\\resources\\textures");

    VertexArrayInitializer::SetupCubeNoNormal(m_cubeVAO);
    VertexArrayInitializer::SetupPlane(m_planeVAO);
    VertexArrayInitializer::SetupTransparent(m_transparentVAO);

    windowsPos = vector<glm::vec3>
    {
        glm::vec3(-1.5f, 0.0f, -0.48f),
        glm::vec3(1.5f, 0.0f, 0.51f),
        glm::vec3(0.0f, 0.0f, 0.7f),
        glm::vec3(-0.3f, 0.0f, -2.3f),
        glm::vec3(0.5f, 0.0f, -0.6f)
    };

    m_shader.Use();
    m_shader.SetInt("texture1", 0);
    m_shader.SetVec3("objectColor", glm::vec3(1.0, 0.0, 0.0));
}

void TestScene::Draw(const Camera& camera)
{
    // sort the transparent windows before rendering
// ---------------------------------------------
    std::map<float, glm::vec3> sorted;
    for (unsigned int i = 0; i < windowsPos.size(); i++)
    {
        float distance = glm::length(camera.Position - windowsPos[i]);
        sorted[distance] = windowsPos[i];
    }

    glm::mat4 model = glm::mat4(1.0f);

    m_shader.Use();

    // cubes
    glBindVertexArray(m_cubeVAO);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, m_cubeTexture);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    m_shader.SetMVPMatrix(model, camera.GetViewMatrix(), camera.GetPerspectiveProj());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    m_shader.SetMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // floor
    //glBindVertexArray(m_planeVAO);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, m_floorTexture);
    //model = glm::mat4(1.0f);
    //m_shader.SetMat4("model", model);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
}
