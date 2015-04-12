#pragma once

#include <glm/glm.hpp>
#include "GameModel.h"

namespace tdogl {

    enum objectType
    {
        OBJECT_SCENERY,
        OBJECT_BATTERY_UNCOLLECTED,
        OBJECT_BATTERY_COLLECTED,
        OBJECT_GOAL
    };
    
    class GameObject
    {
        tdogl::GameModel *model;
        tdogl::RenderingHelper transformation;
        glm::vec3 location;
        objectType type;
        
    public:
        GameObject();
        GameObject(tdogl::GameModel *model, objectType type, glm::vec3 location);
        GameObject(tdogl::GameModel *model, objectType type, glm::vec3 location, glm::vec3 rotation);
        GameObject(tdogl::GameModel *model, objectType type, glm::vec3 location, glm::vec3 rotation, glm::vec3 scale);
        GameModel *getModel();
        glm::mat4 getTransformation();
        glm::vec3 getLocation();
        void rotateObject(float theta, float dx, float dy, float dz);
        void setObjectType(objectType type);
        objectType getObjectType();
    };
}