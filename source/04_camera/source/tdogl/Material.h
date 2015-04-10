#pragma once

#include <glm/glm.hpp>

namespace tdogl {
    struct Material {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
        Material();
        Material(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, float shininessValue);
    };
}
