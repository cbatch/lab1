#include <cassert>
#include <GL/glew.h>
#include <iostream>

#include "glm/glm.hpp"
#include "MeshLoader.h"
#include "tiny_obj_loader.h"
#include "platform.hpp"

using namespace std;

namespace mshLdr
{
	void Mesh::resizeObj(){
			float minX, minY, minZ;
			float maxX, maxY, maxZ;
			float scaleX, scaleY, scaleZ;
			float shiftX, shiftY, shiftZ;
			float epsilon = 0.001;

			minX = minY = minZ = 1.1754E+38F;
			maxX = maxY = maxZ = -1.1754E+38F;

			//Go through all vertices to determine min and max of each dimension
			for (size_t i = 0; i < shapes.size(); i++) {
				for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
					if (shapes[i].mesh.positions[3 * v + 0] < minX) minX = shapes[i].mesh.positions[3 * v + 0];
					if (shapes[i].mesh.positions[3 * v + 0] > maxX) maxX = shapes[i].mesh.positions[3 * v + 0];

					if (shapes[i].mesh.positions[3 * v + 1] < minY) minY = shapes[i].mesh.positions[3 * v + 1];
					if (shapes[i].mesh.positions[3 * v + 1] > maxY) maxY = shapes[i].mesh.positions[3 * v + 1];

					if (shapes[i].mesh.positions[3 * v + 2] < minZ) minZ = shapes[i].mesh.positions[3 * v + 2];
					if (shapes[i].mesh.positions[3 * v + 2] > maxZ) maxZ = shapes[i].mesh.positions[3 * v + 2];
				}
			}
			//From min and max compute necessary scale and shift for each dimension
			float maxExtent, xExtent, yExtent, zExtent;
			xExtent = maxX - minX;
			yExtent = maxY - minY;
			zExtent = maxZ - minZ;
			if (xExtent >= yExtent && xExtent >= zExtent) {
				maxExtent = xExtent;
			}
			if (yExtent >= xExtent && yExtent >= zExtent) {
				maxExtent = yExtent;
			}
			if (zExtent >= xExtent && zExtent >= yExtent) {
				maxExtent = zExtent;
			}
			scaleX = 2.0 / maxExtent;
			shiftX = minX + (xExtent / 2.0);
			scaleY = 2.0 / maxExtent;
			shiftY = minY + (yExtent / 2.0);
			scaleZ = 2.0 / maxExtent;
			shiftZ = minZ + (zExtent) / 2.0;

			//Go through all verticies shift and scale them
			for (size_t i = 0; i < shapes.size(); i++) {
				for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
					shapes[i].mesh.positions[3 * v + 0] = (shapes[i].mesh.positions[3 * v + 0] - shiftX) * scaleX;
					assert(shapes[i].mesh.positions[3 * v + 0] >= -1.0 - epsilon);
					assert(shapes[i].mesh.positions[3 * v + 0] <= 1.0 + epsilon);
					shapes[i].mesh.positions[3 * v + 1] = (shapes[i].mesh.positions[3 * v + 1] - shiftY) * scaleY;
					assert(shapes[i].mesh.positions[3 * v + 1] >= -1.0 - epsilon);
					assert(shapes[i].mesh.positions[3 * v + 1] <= 1.0 + epsilon);
					shapes[i].mesh.positions[3 * v + 2] = (shapes[i].mesh.positions[3 * v + 2] - shiftZ) * scaleZ;
					assert(shapes[i].mesh.positions[3 * v + 2] >= -1.0 - epsilon);
					assert(shapes[i].mesh.positions[3 * v + 2] <= 1.0 + epsilon);
				}
			}
		}
	void Mesh::bindBuffers()
	{
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        
		for (int i = 0; i < shapes.size(); i++)
		{
			tinyobj::mesh_t currShape = shapes[i].mesh;

			const vector<float> &posTemp = shapes[i].mesh.positions;
			GLuint temp;
			glGenBuffers(1, &temp);
			glBindBuffer(GL_ARRAY_BUFFER, temp);
			glBufferData(GL_ARRAY_BUFFER, posTemp.size()*sizeof(float), &posTemp[0], GL_STATIC_DRAW);
			posBufferObjects.push_back(temp);
			
			if (currShape.normals.empty())
			{
                vector<float> norBuf;
				for (int j = 0; j < currShape.positions.size() / 3; j++) {
					norBuf.push_back(0);
					norBuf.push_back(0);
					norBuf.push_back(0);
				}

				for (int j = 0; j < currShape.indices.size() / 3; j++) {
					unsigned int idx1 = currShape.indices[3 * j + 0];
					unsigned int idx2 = currShape.indices[3 * j + 1];
					unsigned int idx3 = currShape.indices[3 * j + 2];
					glm::vec3 v1 = glm::vec3(currShape.positions[3 * idx1 + 0], currShape.positions[3 * idx1 + 1], currShape.positions[3 * idx1 + 2]);
					glm::vec3 v2 = glm::vec3(currShape.positions[3 * idx2 + 0], currShape.positions[3 * idx2 + 1], currShape.positions[3 * idx2 + 2]);
					glm::vec3 v3 = glm::vec3(currShape.positions[3 * idx3 + 0], currShape.positions[3 * idx3 + 1], currShape.positions[3 * idx3 + 2]);

					glm::vec3 cross = glm::cross(v1 - v2, v1 - v3);

					norBuf[3 * idx1 + 0] += cross.x;
					norBuf[3 * idx1 + 1] += cross.y;
					norBuf[3 * idx1 + 2] += cross.z;
					norBuf[3 * idx2 + 0] += cross.x;
					norBuf[3 * idx2 + 1] += cross.y;
					norBuf[3 * idx2 + 2] += cross.z;
					norBuf[3 * idx3 + 0] += cross.x;
					norBuf[3 * idx3 + 1] += cross.y;
					norBuf[3 * idx3 + 2] += cross.z;
				}

				for (int j = 0; j < norBuf.size(); j += 3)
				{
					glm::vec3 normal = glm::vec3(norBuf[j + 0], norBuf[j + 1], norBuf[j + 2]);
					glm::vec3 normalized = glm::normalize(normal);
					norBuf[j + 0] = normalMode * normalized.x;
                    norBuf[j + 1] = normalMode * normalized.y;
                    norBuf[j + 2] = normalMode * normalized.z;
				}
				glGenBuffers(1, &temp);
				glBindBuffer(GL_ARRAY_BUFFER, temp);
				glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
				norBufferObjects.push_back(temp);
			}
            else
            {
                vector<float> &norBuf = shapes[i].mesh.normals;
                glGenBuffers(1, &temp);
                glBindBuffer(GL_ARRAY_BUFFER, temp);
                glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
                norBufferObjects.push_back(temp);
            }

            const vector<unsigned int> &idxTemp = shapes[i].mesh.indices;
            glGenBuffers(1, &temp);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxTemp.size()*sizeof(unsigned int), &idxTemp[0], GL_STATIC_DRAW);
            idxBufferObjects.push_back(temp);

            glBindVertexArray(0);
            
            //assert(GLSL::printError() == GL_NO_ERROR);
		}
	}
	//void Mesh::bindBuffers()
	//{
	//	for (int i = 0; i < shapes.size(); i++)
	//	{
	//		const vector<float> &posTemp = shapes[i].mesh.positions;
	//		posBuffers.push_back(posTemp);
	//		posBufferObjects.push_back(0);
	//		glGenBuffers(1, &(posBufferObjects[i]));
	//		glBindBuffer(GL_ARRAY_BUFFER, posBufferObjects[i]);
	//		glBufferData(GL_ARRAY_BUFFER, posBuffers[i].size()*sizeof(float), &(posBuffers[i]), GL_STATIC_DRAW);

	//		const vector<float> &norTemp = shapes[i].mesh.normals;
	//		norBuffers.push_back(norTemp);
	//		norBufferObjects.push_back(0);
	//		glGenBuffers(1, &(norBufferObjects[i]));
	//		glBindBuffer(GL_ARRAY_BUFFER, norBufferObjects[i]);
	//		glBufferData(GL_ARRAY_BUFFER, norBuffers[i].size()*sizeof(float), &(norBuffers[i]), GL_STATIC_DRAW);

	//		const vector<unsigned int> &idxTemp = shapes[i].mesh.indices;
	//		idxBuffers.push_back(idxTemp);
	//		idxBufferObjects.push_back(0);
	//		glGenBuffers(1, &(idxBufferObjects[i]));
	//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxBufferObjects[i]);
	//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxBuffers[i].size()*sizeof(float), &(idxBuffers[i]), GL_STATIC_DRAW);
	//	}
	//}
	Mesh::Mesh() {}
    Mesh::Mesh(std::string objectFile) : Mesh(objectFile, MESH_OUTSIDE_NORMALS)	{}
    Mesh::Mesh(std::string objectFile, meshNormalMode normalMode)
    {
        this->normalMode = normalMode;
        string err = tinyobj::LoadObj(shapes, materials, ResourcePath(objectFile).c_str());
        if (!err.empty()) {
            std::cerr << err << endl;
        }
        resizeObj();
        //calculateNormals();
        bindBuffers();
		cout << shapes[0].mesh.positions.size() << "\n";
    }
    int Mesh::getShapeCount()
    {
        return shapes.size();
    }
	tinyobj::shape_t Mesh::getShape(int index)
	{
		return shapes[index];
	}
	GLuint Mesh::getPosBufferObject(int index)
	{
		return posBufferObjects[index];
	}
	GLuint Mesh::getNorBufferObject(int index)
	{
		return norBufferObjects[index];
	}
	GLuint Mesh::getIdxBufferObject(int index)
	{
		return idxBufferObjects[index];
	}
	int Mesh::indexCount(int shape)
	{
		return shapes[shape].mesh.indices.size();
	}

	MeshLoader::MeshLoader()
	{
		meshes = map<std::string, Mesh>();
	}		
	void MeshLoader::LoadMeshes(vector<std::string> objectFiles)
	{
        for(int i = 0; i < objectFiles.size(); i++)
        {
			meshes.insert(std::pair<std::string, Mesh>(objectFiles[i].erase(objectFiles[i].length() - 4, objectFiles[i].length()), Mesh(objectFiles[i])));
		}
	}
    void MeshLoader::LoadMeshes(vector<std::string> objectFiles, meshNormalMode normalMode)
    {
        for(int i = 0; i < objectFiles.size(); i++)
        {
            meshes.insert(std::pair<std::string, Mesh>(objectFiles[i].erase(objectFiles[i].length() - 4, objectFiles[i].length()), Mesh(objectFiles[i], normalMode)));
        }
    }
	Mesh * MeshLoader::getMesh(std::string meshName)
	{
		return &(meshes.at(meshName));
	}
    void Mesh::setNormalMode(meshNormalMode normalMode)
    {
        this->normalMode = normalMode;
    }
    int Mesh::getNormalMode()
    {
        return normalMode;
    }
}