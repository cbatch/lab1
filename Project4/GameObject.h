#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <glm/glm.hpp>
#include "GameModel.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "glm/gtx/string_cast.hpp"
using namespace glm;

enum objectType
{
    OBJECT_SCENERY,
    OBJECT_ITEM
};

class GameObject
{
    GameModel *model;
    mat4 translation;
    mat4 rotation;
    mat4 scale;
    vec3 location;
    objectType type;
    double currentTime;
    bool butterfly_net_captured;
    double lemon_time;
    int lemon_deque_position;
    int lemon_offset;

public:
    vec3 velocity;

    GameObject();
    GameObject(GameModel *model, objectType type, vec3 location);
    GameObject(GameModel *model, objectType type, vec3 location, vec3 rotation);
    GameObject(GameModel *model, objectType type, vec3 location, vec3 rotation, vec3 scale);
    GameModel *getModel();
    mat4 getTransformation();
    vec3 getLocation();
    void rotateObject(float theta, float dx, float dy, float dz);
    void setObjectType(objectType type);
    bool step(void (*delete_coin)());
    void collected_via_butterfly_net();
    objectType getObjectType();
};

#endif