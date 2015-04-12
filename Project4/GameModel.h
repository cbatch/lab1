#ifndef GAME_MODEL_H
#define GAME_MODEL_H

#include "Material.h"
#include "MeshLoader.h"
#include "RenderingHelper.h"
using namespace mshLdr;
using namespace std;

class GameModel
{
	Mesh mesh;
	vector<Material> materials;
	vec3 center;
	float collisionRadius;
	RenderingHelper transformation;
	
public:
	GameModel();
	GameModel(Mesh mesh);
	GameModel(Mesh mesh, vec3 center);
	GameModel(Mesh mesh, vec3 center, float radius);
	GameModel(Mesh mesh, float radius);
	void addMesh(Mesh mesh);
    void addMaterial(Material material);
	vec3 getCenter();
	void setCenter(vec3 newCenter);
	float getCollisionRadius();
	void setCollisionRadius(float newRadius);
	mat4 getTransformationMatrix();
	void resetTransformation();
	void translate(float dx, float dy, float dz);
	void rotate(float theta, float dx, float dy, float dz);
	void scale(float uniform);
	void scale(float sx, float sy, float sz);
    int getCount();
	Mesh getMesh();
	Material getMaterial(int index);
    vector<Material> getMaterials();
};

#endif