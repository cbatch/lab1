#include "GameObject.h"

GameObject::GameObject() : currentTime(glfwGetTime()), butterfly_net_captured(false)
{
    translation = mat4(1.0f);
    rotation = mat4(1.0f);
    scale = mat4(1.0f);
}

GameObject::GameObject(GameModel *model, objectType type, vec3 location) : GameObject()
{
    this->model = model;
    this->location = location;
    this->type = type;
    translation *= translate(location);
}

GameObject::GameObject(GameModel *model, objectType type, vec3 location, vec3 rotation) : GameObject(model, type, location)
{
    this->rotation *= rotate(rotation.y, vec3(0, 1, 0));
    this->rotation *= rotate(rotation.x, vec3(1, 0, 0));
    this->rotation *= rotate(rotation.z, vec3(0, 0, 1));
}

GameObject::GameObject(GameModel *model, objectType type, vec3 location, vec3 rotation, vec3 scale) : GameObject(model, type, location, rotation)
{
    this->scale *= glm::scale(scale);
}

GameModel *GameObject::getModel()
{
    return model;
}

mat4 GameObject::getTransformation()
{
    return translation * rotation * scale;
}

void GameObject::rotateObject(float theta, float dx, float dy, float dz)
{
    rotation *= rotate(theta, vec3(dx, dy, dz));
}

vec3 GameObject::getLocation()
{
    return vec3(translation[3][0], translation[3][1], translation[3][2]);
}

void GameObject::setObjectType(objectType type)
{
    this->type = type;
}

objectType GameObject::getObjectType()
{
    return type;
}

void GameObject::collected_via_butterfly_net()
{
    velocity = vec3(0.f, 1.0f, 0.0f);
    butterfly_net_captured = true;
    lemon_time = glfwGetTime();
}

bool GameObject::step(void(*delete_coin)())
{
    if (type == OBJECT_SCENERY)
        return false;

    double newTime = glfwGetTime();

    if (butterfly_net_captured && newTime - lemon_time > 2)
    {
        delete_coin();
    }

    float frameTime = newTime - currentTime;
    if (frameTime > 0.01)
    {
        if (!butterfly_net_captured)
            rotation *= rotate(5.f * frameTime, vec3(0.f, 1.0f, 0.f));
        else
            rotation *= rotate(25.f * frameTime, vec3(0.f, 1.0f, 0.f));
		
		vec3 vec = (frameTime * velocity);
		translation[3][0] += vec.x;
		translation[3][1] += vec.y;
		translation[3][2] += vec.z;
        //translation *= translate(0.01f * velocity);
        currentTime = newTime;


        if (abs(translation[3][0]) > 10.0f || abs(translation[3][2]) > 10.0f)
        {
            velocity *= -1.0f;
        }
        return true;
    }
    return false;
}
