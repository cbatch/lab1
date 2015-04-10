#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <stdio.h>
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Record.h"
#include "Program.h"
#include "Shader.h"
#include "platform.hpp"

namespace tdogl {
    
    class Mesh {
    public:
        Mesh(const std::string &objFile, std::shared_ptr<std::vector<tdogl::Shader>> shaders);
        void draw(glm::mat4 camera);
        void init();
        Mesh& with(Record *record);
    protected:
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        GLuint posBufObj;
        GLuint norBufObj;
        GLuint indBufObj;
        GLuint texBufObj;
        GLuint gVAO = 0;
        Record* record;
        Program program;
    private:
        const std::string objFile;
        void load_shapes();
        void resize_obj();
        void initialize_buffers();
    };
}
        

