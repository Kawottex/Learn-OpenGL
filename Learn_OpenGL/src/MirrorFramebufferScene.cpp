#include "MirrorFramebufferScene.h"
#include <VertexArrayInitializer.h>
#include <Model.h>
#include <map>
#include <stb_image.h>

MirrorFramebufferScene::MirrorFramebufferScene()
{
}

void MirrorFramebufferScene::Setup()
{
    VertexArrayInitializer::SetupCube(m_cubeVAO);
    VertexArrayInitializer::SetupPlane(m_planeVAO);
    VertexArrayInitializer::Setup3DQuad(m_quadVAO);
    VertexArrayInitializer::SetupScreenQuad(m_screenQuadVAO);
    VertexArrayInitializer::SetupMirrorQuad(m_mirrorQuadVAO);

    m_shader = Shader(".\\shaders\\depth_testing.vs", ".\\shaders\\depth_testing.fs");
    m_borderShader = Shader(".\\shaders\\depth_testing.vs", ".\\shaders\\shaderSingleColor.fs");
    m_screenShader = Shader(".\\shaders\\screenShader.vs", ".\\shaders\\screenShader.fs");

    m_cubeTexture = Model::TextureFromFile("marble.jpg", ".\\resources\\textures");
    m_floorTexture = Model::TextureFromFile("metal.png", ".\\resources\\textures");
    m_windowTexture = Model::TextureFromFile("blending_transparent_window.png", ".\\resources\\textures");

    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_quadArrayPos.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
    m_quadArrayPos.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
    m_quadArrayPos.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
    m_quadArrayPos.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
    m_quadArrayPos.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

    SetupFramebuffer(m_framebuffer, m_texColorbuffer);
    SetupFramebuffer(m_mirrorFramebuffer, m_mirrorTexColorbuffer);
}

void MirrorFramebufferScene::Draw(const Camera& camera)
{
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Setup scene framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // Draw scene in framebuffer
    glm::mat4 model = glm::mat4(1.0f);
    m_shader.Use();
    m_shader.SetMVPMatrix(model, camera.GetViewMatrix(), camera.GetPerspectiveProj());
    m_borderShader.Use();
    m_borderShader.SetMVPMatrix(model, camera.GetViewMatrix(), camera.GetPerspectiveProj());

    // Draw scene
    DrawFloor(m_shader, m_planeVAO, m_floorTexture);
    DrawCubes(m_shader, m_cubeVAO, m_cubeTexture);
    DrawQuadArray(m_shader, m_quadVAO, m_windowTexture, m_quadArrayPos, camera);

    // Setup mirror framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_mirrorFramebuffer);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // Draw mirrored scene in framebuffer
    model = glm::mat4(1.0f);
    m_shader.Use();
    m_shader.SetMVPMatrix(model, GetInvertedView(camera), camera.GetPerspectiveProj());
    DrawFloor(m_shader, m_planeVAO, m_floorTexture);
    DrawCubes(m_shader, m_cubeVAO, m_cubeTexture);
    DrawQuadArray(m_shader, m_quadVAO, m_windowTexture, m_quadArrayPos, camera);

    // Draw screen quads
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    DrawScreenQuad(m_screenShader, m_screenQuadVAO, m_texColorbuffer);
    DrawScreenQuad(m_screenShader, m_mirrorQuadVAO, m_mirrorTexColorbuffer);
}

void MirrorFramebufferScene::SetupFramebuffer(unsigned int& fbo, unsigned int& texColorBuffer)
{
    // framebuffer configuration
    // -------------------------
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // create a color attachment texture
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800.0f, 600.0f, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800.0f, 600.0f); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MirrorFramebufferScene::DrawFloor(Shader& shader, unsigned int planeVAO, unsigned int floorTexture)
{
    glStencilMask(0x00);

    shader.Use();
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    shader.SetMat4("model", glm::mat4(1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void MirrorFramebufferScene::DrawCubes(Shader& shader, unsigned int cubeVAO, unsigned int cubeTexture)
{
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    shader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    shader.SetMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    shader.SetMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void MirrorFramebufferScene::DrawQuadArray(Shader& shader, unsigned int quadVAO, unsigned int texture, const std::vector<glm::vec3>& quadArray, const Camera& camera)
{
    std::map<float, glm::vec3> sorted;
    for (unsigned int i = 0; i < quadArray.size(); i++)
    {
        float distance = glm::length(camera.Position - quadArray[i]);
        sorted[distance] = quadArray[i];
    }

    shader.Use();

    glBindVertexArray(quadVAO);
    glBindTexture(GL_TEXTURE_2D, texture);

    glm::mat4 model = glm::mat4(1.0f);
    for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it !=
        sorted.rend(); ++it)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, it->second);
        shader.SetMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void MirrorFramebufferScene::DrawScreenQuad(Shader& shader, unsigned int quadVAO, unsigned int texture)
{
    shader.Use();
    glBindVertexArray(quadVAO);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

glm::mat4 MirrorFramebufferScene::GetInvertedView(const Camera& camera)
{
    Camera tmpCamera = Camera(camera);
    tmpCamera.Front *= -1;
    glm::mat4 view = tmpCamera.GetViewMatrix();
    return view;
}