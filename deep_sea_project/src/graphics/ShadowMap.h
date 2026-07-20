#pragma once

#include <glad/glad.h>

class ShadowMap
{
public:

    unsigned int depthMapFBO;
    unsigned int depthMap;

    const unsigned int SHADOW_WIDTH = 4096;
    const unsigned int SHADOW_HEIGHT = 4096;

    ShadowMap();

    void Bind();
    void Unbind();
};