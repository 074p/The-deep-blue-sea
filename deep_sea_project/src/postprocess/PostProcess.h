#pragma once

#include <glad/glad.h>

#include "../Shader.h"

class PostProcess
{
public:

    unsigned int FBO;
    unsigned int textureColorBuffer;
    unsigned int RBO;

    unsigned int quadVAO;
    unsigned int quadVBO;

    Shader* shader;

    int width;
    int height;

    PostProcess(
        int screenWidth,
        int screenHeight
    );

    void BeginRender();

    void EndRender();

    void Render();
};