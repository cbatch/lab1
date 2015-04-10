#include "BunnyRecord.h"

using namespace tdogl;

glm::mat4 BunnyRecord::getModelMat()
{
    return glm::scale(glm::mat4(1.0f), glm::vec3(0.1, 0.1, 0.1));
}