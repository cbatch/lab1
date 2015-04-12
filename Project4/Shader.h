#ifndef SHADER_H
#define SHADER_H

#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "GLSL.h"
using namespace std;

class Shader
{
    GLuint program;
    GLuint vertexShader;
    GLuint fragmentShader;
    map<string, GLint> attributes;
    map<string, GLint> uniforms;

public:
    Shader();
    Shader(string vertShader, string fragShader);
    void setupHandles();
    GLint getAttributeHandle(string name);
    GLint getUniformHandle(string name);
    GLuint getProgramID();
};

#endif