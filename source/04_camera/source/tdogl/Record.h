#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr

namespace tdogl {
    class Record
    {
    public:
        Record();
        static float rand_FloatRange(float a, float b);
        virtual glm::mat4 getModelMat() =0;
        void setViewMat(glm::mat4 view);
        void setProjMat(glm::mat3 proj);
    private:
        glm::mat4 modelMat;
        glm::mat4 viewMat;
        glm::mat4 projMat;
    };
}