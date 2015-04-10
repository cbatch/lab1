#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr
#include "Record.h"

namespace tdogl {
    class BunnyRecord : public Record
    {
        virtual glm::mat4 getModelMat();
    };
}
