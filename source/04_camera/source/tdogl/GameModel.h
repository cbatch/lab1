#pragma once

#include "Material.h"
#include "MeshLoader.h"
#include "RenderingHelper.h"

namespace tdogl {
    class GameModel
    {
        mshLdr::Mesh mesh;
        std::vector<Material> materials;
        glm::vec3 center;
        float collisionRadius;
        tdogl::RenderingHelper transformation;
        
    public:
        GameModel();
        GameModel(mshLdr::Mesh mesh);
        GameModel(mshLdr::Mesh mesh, glm::vec3 center);
        GameModel(mshLdr::Mesh mesh, glm::vec3 center, float radius);
        GameModel(mshLdr::Mesh mesh, float radius);
        void addMesh(mshLdr::Mesh mesh);
        void addMaterial(tdogl::Material material);
        glm::vec3 getCenter();
        void setCenter(glm::vec3 newCenter);
        float getCollisionRadius();
        void setCollisionRadius(float newRadius);
        glm::mat4 getTransformationMatrix();
        void resetTransformation();
        void translate(float dx, float dy, float dz);
        void rotate(float theta, float dx, float dy, float dz);
        void scale(float uniform);
        void scale(float sx, float sy, float sz);
        int getCount();
        mshLdr::Mesh getMesh();
        tdogl::Material getMaterial(int index);
        std::vector<Material> getMaterials();
    };
}