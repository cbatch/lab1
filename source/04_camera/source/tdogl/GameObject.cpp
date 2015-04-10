#include "GameObject.h"
using namespace tdogl;
using namespace glm;
using namespace std;
GameObject::GameObject()
{
    transformation.useModelViewMatrix();
    transformation.loadIdentity();
}

GameObject::GameObject(GameModel *model, objectType type, vec3 location) : GameObject()
{
    this->model = model;
    this->location = location;
    this->type = type;
    transformation.translate(location);
}

GameObject::GameObject(GameModel *model, objectType type, vec3 location, vec3 rotation) : GameObject(model, type, location)
{
    transformation.rotate(rotation.y, vec3(0, 1, 0));
    transformation.rotate(rotation.x, vec3(1, 0, 0));
    transformation.rotate(rotation.z, vec3(0, 0, 1));
}

GameObject::GameObject(GameModel *model, objectType type, vec3 location, vec3 rotation, vec3 scale) : GameObject(model, type, location, rotation)
{
    transformation.scale(scale.x, scale.y, scale.z);
}

GameModel *GameObject::getModel()
{
    return model;
}

mat4 GameObject::getTransformation()
{
    return transformation.modelViewMatrix;
}

void GameObject::rotateObject(float theta, float dx, float dy, float dz)
{
    transformation.rotate(theta, vec3(dx, dy, dz));
}

vec3 GameObject::getLocation()
{
    return location - model->getCenter();
}

void GameObject::setObjectType(objectType type)
{
    this->type = type;
}

objectType GameObject::getObjectType()
{
    return type;
}
