#include "PostProcess.h"
#include <iostream>
float quadVertices[] =
{
    // positions   // texCoords

    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

PostProcess::PostProcess(
    int screenWidth,
    int screenHeight
)
{
    width = screenWidth;
    height = screenHeight;

    shader = new Shader(
        "shaders/postprocess.vert",
        "shaders/postprocess.frag"
    );

    // =====================================
    // FRAMEBUFFER
    // =====================================

    glGenFramebuffers(1, &FBO);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // color texture

    glGenTextures(1, &textureColorBuffer);

    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        width,
        height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        NULL
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        textureColorBuffer,
        0
    );

    // renderbuffer

    glGenRenderbuffers(1, &RBO);

    glBindRenderbuffer(GL_RENDERBUFFER, RBO);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,
        width,
        height
    );

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        RBO
    );

    // framebuffer check

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER)
        != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "PostProcess Framebuffer not complete\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // =====================================
    // SCREEN QUAD
    // =====================================

    glGenVertexArrays(1, &quadVAO);

    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(quadVertices),
        quadVertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)(2 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void PostProcess::BeginRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcess::EndRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcess::Render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT);

    shader->use();

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(
        GL_TEXTURE_2D,
        textureColorBuffer
    );

    glUniform1i(
        glGetUniformLocation(
            shader->ID,
            "screenTexture"
        ),
        0
    );

    glBindVertexArray(quadVAO);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}