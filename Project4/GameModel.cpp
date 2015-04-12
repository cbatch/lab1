#include "GameModel.h"
#include <iostream>

GameModel::GameModel() 
{
    mesh = Mesh();
    materials = vector<Material>();
    resetTransformation();
    center = vec3(0.0f);
    collisionRadius = 0.0;
};

GameModel::GameModel(Mesh mesh) : GameModel()
{
    this->mesh = mesh;
};

GameModel::GameModel(Mesh mesh, vec3 center) : GameModel(mesh)
{
	this->center = center;
};

GameModel::GameModel(Mesh mesh, vec3 center, float radius) : GameModel(mesh, center)
{
	collisionRadius = radius;
};

GameModel::GameModel(Mesh mesh, float radius) : GameModel(mesh, vec3(0.0f), radius){};

void GameModel::addMaterial(Material material)
{
    materials.push_back(material);
}

vec3 GameModel::getCenter()
{
	return center;
}

void GameModel::setCenter(vec3 newCenter)
{
	center = newCenter;
}

float GameModel::getCollisionRadius()
{
	return collisionRadius;
}

void GameModel::setCollisionRadius(float newRadius)
{
	collisionRadius = newRadius;
}

mat4 GameModel::getTransformationMatrix()
{
	return transformation.modelViewMatrix;
}

void GameModel::resetTransformation()
{
    transformation.useModelViewMatrix();
    transformation.loadIdentity();
}

void GameModel::translate(float dx, float dy, float dz)
{
	transformation.translate(vec3(dx, dy, dz));
}

void GameModel::rotate(float theta, float dx, float dy, float dz)
{
	transformation.rotate(theta, vec3(dx, dy, dz));
}

void GameModel::scale(float uniform)
{
	transformation.scale(uniform);
}

void GameModel::scale(float sx, float sy, float sz)
{
	transformation.scale(sx, sy, sz);
}

Mesh GameModel::getMesh()
{
	return mesh;
}

Material GameModel::getMaterial(int index)
{
    Material material;
    try
    {
        material = materials.at(index);
    }
    catch (exception& e)
    {
        cout << "Material at index '" << index << "' is not found for this model." << endl;
        assert(false);
    }
    return material;
}

vector<Material> GameModel::getMaterials()
{
    return materials;
}