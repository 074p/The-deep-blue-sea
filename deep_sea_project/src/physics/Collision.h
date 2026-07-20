#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Triangle
{
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;
};

class Collision
{
public:

    static glm::vec3 ClosestPointTriangle(
        const glm::vec3& p,
        const Triangle& tri
    );

    static bool SphereTriangleCollision(
        const glm::vec3& center,
        float radius,
        const Triangle& tri
    );
};