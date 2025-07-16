#include "CubemapScene.h"
#include <VertexArrayInitializer.h>
#include <Model.h>
#include <stb_image.h>

CubemapScene::CubemapScene()
{
}

void CubemapScene::Setup()
{
    m_bEnableFramebuffer = false;

    VertexArrayInitializer::SetupCube(m_cubeReflectionVAO);
    VertexArrayInitializer::SetupCube(m_cubeVAO);
    VertexArrayInitializer::SetupPlane(m_planeVAO);
    VertexArrayInitializer::Setup3DQuad(m_quadVAO);
    VertexArrayInitializer::SetupScreenQuad(m_screenQuadVAO);
    VertexArrayInitializer::SetupCubeNoTexture(m_skyboxVAO);

    m_shader = Shader(".\\shaders\\enviroMappingShader.vs", ".\\shaders\\enviroMappingShader.fs");
    m_borderShader = Shader(".\\shaders\\depth_testing.vs", ".\\shaders\\shaderSingleColor.fs");
    m_screenShader = Shader(".\\shaders\\screenShader.vs", ".\\shaders\\screenShader.fs");
    m_skyboxShader = Shader(".\\shaders\\skyboxShader.vs", ".\\shaders\\skyboxShader.fs");

    m_cubeTexture = Model::TextureFromFile("marble.jpg", ".\\resources\\textures");

    std::vector<std::string> faces
    {
        ".\\resources\\textures\\skybox\\right.jpg",
        ".\\resources\\textures\\skybox\\left.jpg",
        ".\\resources\\textures\\skybox\\top.jpg",
        ".\\resources\\textures\\skybox\\bottom.jpg",
        ".\\resources\\textures\\skybox\\front.jpg",
        ".\\resources\\textures\\skybox\\back.jpg"
    };

    m_cubemapTexture = LoadCubemap(faces);

    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    vector<glm::vec3> quadArrayPos
    {
        glm::vec3(-1.5f, 0.0f, -0.48f),
        glm::vec3(1.5f, 0.0f, 0.51f),
        glm::vec3(0.0f, 0.0f, 0.7f),
        glm::vec3(-0.3f, 0.0f, -2.3f),
        glm::vec3(0.5f, 0.0f, -0.6f)
    };

    SetupFramebuffer();
}

void CubemapScene::Draw(const Camera& camera)
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_bEnableFramebuffer)
    {
        // Setup scene framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        glEnable(GL_DEPTH_TEST);
    }

    m_shader.Use();
    m_shader.SetVec3("cameraPos", camera.Position);

    // Skybox
    DrawSkybox(m_skyboxShader, m_skyboxVAO, m_cubemapTexture, camera);

    // Draw scene
    glm::mat4 model = glm::mat4(1.0f);
    m_shader.Use();
    m_shader.SetMVPMatrix(model, camera.GetViewMatrix(), camera.GetPerspectiveProj());
    m_borderShader.Use();
    m_borderShader.SetMVPMatrix(model, camera.GetViewMatrix(), camera.GetPerspectiveProj());
    DrawCubes(m_shader, m_cubeReflectionVAO, m_cubeTexture);

    if (m_bEnableFramebuffer)
    {
        // Draw screen quads
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        DrawScreenQuad(m_screenShader, m_screenQuadVAO, m_textureColorbuffer);
    }
}

void CubemapScene::SetupFramebuffer()
{
    // framebuffer configuration
    // -------------------------
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    // create a color attachment texture
    glGenTextures(1, &m_textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, m_textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800.0f, 600.0f, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureColorbuffer, 0);

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

unsigned int CubemapScene::LoadCubemap(const std::vector<string>& faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    unsigned char* data;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width,
                height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            std::cout << "Cubemap failed to load at path: " << faces[i] << std::endl;
        }
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void CubemapScene::DrawScreenQuad(Shader& shader, unsigned int quadVAO, unsigned int texture)
{
    shader.Use();
    glBindVertexArray(quadVAO);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void CubemapScene::DrawCubes(Shader& shader, unsigned int cubeVAO, unsigned int cubeTexture)
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

void CubemapScene::DrawSkybox(Shader& shader, unsigned int skyboxVAO, unsigned int cubemapTexture, const Camera& camera)
{
    glDepthFunc(GL_LEQUAL);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 viewNoTranslate = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    shader.Use();
    shader.SetMVPMatrix(model, viewNoTranslate, camera.GetPerspectiveProj());
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_LESS);
}