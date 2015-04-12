#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
using namespace glm;

struct Material {
    vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	Material();
    Material(vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, float shininessValue);
};

#endif