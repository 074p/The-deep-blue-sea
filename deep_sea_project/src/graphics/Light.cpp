#include "Light.h"

Light::Light()
{
    position = glm::vec3(0.0f, -5.0f, 10.0f);

    ambient = glm::vec3(0.25f);
    diffuse = glm::vec3(1.0f);
    specular = glm::vec3(1.0f);

    constant = 1.0f;
    linear = 0.003f;
    quadratic = 0.0002f;
}