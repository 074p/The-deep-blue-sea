#include "Material.h"

Material::Material()
{
    ambient = glm::vec3(0.8f);
    diffuse = glm::vec3(0.8f);
    specular = glm::vec3(1.0f);

    shininess = 32.0f;
}