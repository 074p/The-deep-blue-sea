#include "Shader.h"

#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(
    const char* vertexPath,
    const char* fragmentPath
)
{
    std::ifstream vFile(vertexPath);

    std::ifstream fFile(fragmentPath);

    std::stringstream vStream;
    std::stringstream fStream;

    vStream << vFile.rdbuf();

    fStream << fFile.rdbuf();

    std::string vertexCode = vStream.str();

    std::string fragmentCode = fStream.str();

    const char* vShaderCode = vertexCode.c_str();

    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex;
    unsigned int fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertex,
        1,
        &vShaderCode,
        NULL
    );

    glCompileShader(vertex);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragment,
        1,
        &fShaderCode,
        NULL
    );

    glCompileShader(fragment);

    ID = glCreateProgram();

    glAttachShader(ID, vertex);

    glAttachShader(ID, fragment);

    glLinkProgram(ID);

    glDeleteShader(vertex);

    glDeleteShader(fragment);
}

void Shader::use()
{
    glUseProgram(ID);
}