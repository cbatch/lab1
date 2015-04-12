#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <glm/glm.hpp>
#include "GameModel.h"
#include "RenderingHelper.h"
using namespace glm;

enum objectType
{
    OBJECT_SCENERY,
    OBJECT_BATTERY_UNCOLLECTED,
    OBJECT_BATTERY_COLLECTED,
    OBJECT_GOAL
};

class GameObject
{
    GameModel *model;
    RenderingHelper transformation;
    vec3 location;
    objectType type;

public:
    GameObject();
    GameObject(GameModel *model, objectType type, vec3 location);
    GameObject(GameModel *model, objectType type, vec3 location, vec3 rotation);
    GameObject(GameModel *model, objectType type, vec3 location, vec3 rotation, vec3 scale);
    GameModel *getModel();
    mat4 getTransformation();
    vec3 getLocation();
    void rotateObject(float theta, float dx, float dy, float dz);
    void setObjectType(objectType type);
    objectType getObjectType();
};

#endif