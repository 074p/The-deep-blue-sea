#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "physics/Collision.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class STLLoader
{
public:

    std::vector<Vertex> vertices;

    std::vector<Triangle> collisionTriangles;

    bool loadBinarySTL(
        const std::string& path
    );
};