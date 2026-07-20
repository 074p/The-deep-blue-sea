#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

Texture::Texture(const std::string& path)
{
    glGenTextures(1,&ID);
    glBindTexture(GL_TEXTURE_2D,ID);

    stbi_set_flip_vertically_on_load(true);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    int width,height,nrChannels;

    unsigned char* data=
    stbi_load(
        path.c_str(),
        &width,
        &height,
        &nrChannels,
        0
    );

    if(data)
    {
        GLenum format=GL_RGB;

        if(nrChannels==1) format=GL_RED;
        else if(nrChannels==3) format=GL_RGB;
        else if(nrChannels==4) format=GL_RGBA;

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );

        glGenerateMipmap(GL_TEXTURE_2D);

        std::cout
        <<"Loaded texture: "
        <<path
        <<" ("<<width<<"x"<<height<<")\n";
    }
    else
    {
        std::cout
        <<"Failed texture: "
        <<path<<"\n";
    }

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D,0);
}