#ifndef _MESH_LOADER_H
#define _MESH_LOADER_H

#include <GL/glew.h>
#include <map>
#include <string>
#include <vector>

#include "tiny_obj_loader.h"

namespace mshLdr
{
    enum meshNormalMode {
        MESH_OUTSIDE_NORMALS = 1,
        MESH_INSIDE_NORMALS = - 1
    };

	class Mesh
	{
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::vector<GLuint> posBufferObjects;
		std::vector<const std::vector<float>> posBuffers;
		std::vector<GLuint> norBufferObjects;
		std::vector<std::vector<float>> norBuffers;
		std::vector<GLuint> idxBufferObjects;
		std::vector<const std::vector<unsigned int>> idxBuffers;
        int normalMode;
		void resizeObj();
		void calculateNormals();
		void bindBuffers();
	public:
		Mesh();
		Mesh(std::string objectFile);
        Mesh(std::string objectFile, meshNormalMode normalMode);
        int getShapeCount();
		tinyobj::shape_t getShape(int index);
		GLuint getPosBufferObject(int index);
		GLuint getNorBufferObject(int index);
		GLuint getIdxBufferObject(int index);
		int indexCount(int shape);
        void setNormalMode(meshNormalMode normalMode);
        int getNormalMode();
	};

	class MeshLoader
	{
		std::map<std::string, Mesh> meshes;
	public:
		MeshLoader();
		void LoadMeshes(std::vector<std::string> objectFiles);
        void LoadMeshes(std::vector<std::string> objectFiles, meshNormalMode normalMode);
		Mesh *getMesh(std::string meshName);
	};
}

#endif