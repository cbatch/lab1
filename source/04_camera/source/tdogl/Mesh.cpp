#include "Mesh.h"

using namespace std;
using namespace tdogl;

Mesh::Mesh(const string &objFile, shared_ptr<vector<Shader>> shaders)
: objFile(objFile), program(shaders)
{
}
void Mesh::init()
{
    load_shapes();
    initialize_buffers();
}
void Mesh::initialize_buffers()
{
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);
    
    const vector<float> &posBuf = shapes[0].mesh.positions;
    glGenBuffers(1, &posBufObj);
    glBindBuffer(GL_ARRAY_BUFFER, posBufObj);
    glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(program.attrib("vert"));
    glVertexAttribPointer(program.attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    // TODO compute the normals per vertex - you must fill this in
    vector<float> norBuf;
    int idx1, idx2, idx3;
    glm::vec3 v1, v2, v3;
    //for every vertex initialize a normal to 0
    for (int j = 0; j < Mesh::shapes[0].mesh.positions.size()/3; j++)
    {
        norBuf.push_back(0);
        norBuf.push_back(0);
        norBuf.push_back(0);
    }
    // DO work here to compute the normals for every face
    //then add its normal to its associated vertex
    for (int i = 0; i < shapes[0].mesh.indices.size()/3; i++) {
        idx1 = shapes[0].mesh.indices[3*i+0];
        idx2 = shapes[0].mesh.indices[3*i+1];
        idx3 = shapes[0].mesh.indices[3*i+2];
        v1 = glm::vec3(shapes[0].mesh.positions[3*idx1 +0],shapes[0].mesh.positions[3*idx1 +1], shapes[0].mesh.positions[3*idx1 +2]);
        v2 = glm::vec3(shapes[0].mesh.positions[3*idx2 +0],shapes[0].mesh.positions[3*idx2 +1], shapes[0].mesh.positions[3*idx2 +2]);
        v3 = glm::vec3(shapes[0].mesh.positions[3*idx3 +0],shapes[0].mesh.positions[3*idx3 +1], shapes[0].mesh.positions[3*idx3 +2]);
        
        //This is not correct, it sets the normal as the vertex value but
        //shows access pattern
        
        glm::vec3 e1 = glm::normalize(v2 - v1);
        glm::vec3 e2 = glm::normalize(v3 - v1);
        
        glm::vec3 faceNorm = glm::normalize(glm::cross(e1, e2));
        
        norBuf[3*idx1+0] += faceNorm.x;
        norBuf[3*idx1+1] += faceNorm.y;
        norBuf[3*idx1+2] += faceNorm.z;
        norBuf[3*idx2+0] += faceNorm.x;
        norBuf[3*idx2+1] += faceNorm.y;
        norBuf[3*idx2+2] += faceNorm.z;
        norBuf[3*idx3+0] += faceNorm.x;
        norBuf[3*idx3+1] += faceNorm.y;
        norBuf[3*idx3+2] += faceNorm.z;
    }
    glGenBuffers(1, &norBufObj);
    glBindBuffer(GL_ARRAY_BUFFER, norBufObj);
    glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(program.attrib("norm"));
    glVertexAttribPointer(program.attrib("norm"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    
    // Send the index array to the GPU
    const vector<unsigned int> &indBuf = shapes[0].mesh.indices;
    glGenBuffers(1, &indBufObj);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBufObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indBuf.size()*sizeof(unsigned int), &indBuf[0], GL_STATIC_DRAW);
    
    const vector<float> &texBuf = shapes[0].mesh.texcoords;
    glGenBuffers(1, &texBufObj);
    glBindBuffer(GL_ARRAY_BUFFER, texBufObj);
    glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW);
    
    glBindVertexArray(0);
}

void Mesh::load_shapes()
{
    cout << ResourcePath(objFile).c_str();
    string err = tinyobj::LoadObj(shapes, materials, ResourcePath(objFile).c_str());
    if(!err.empty()) {
        cerr << err << endl;
    }
    resize_obj();
}

void Mesh::resize_obj(){
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
            if(shapes[i].mesh.positions[3*v+0] < minX) minX = shapes[i].mesh.positions[3*v+0];
            if(shapes[i].mesh.positions[3*v+0] > maxX) maxX = shapes[i].mesh.positions[3*v+0];
            
            if(shapes[i].mesh.positions[3*v+1] < minY) minY = shapes[i].mesh.positions[3*v+1];
            if(shapes[i].mesh.positions[3*v+1] > maxY) maxY = shapes[i].mesh.positions[3*v+1];
            
            if(shapes[i].mesh.positions[3*v+2] < minZ) minZ = shapes[i].mesh.positions[3*v+2];
            if(shapes[i].mesh.positions[3*v+2] > maxZ) maxZ = shapes[i].mesh.positions[3*v+2];
        }
    }
    //From min and max compute necessary scale and shift for each dimension
    float maxExtent, xExtent, yExtent, zExtent;
    xExtent = maxX-minX;
    yExtent = maxY-minY;
    zExtent = maxZ-minZ;
    if (xExtent >= yExtent && xExtent >= zExtent) {
        maxExtent = xExtent;
    }
    if (yExtent >= xExtent && yExtent >= zExtent) {
        maxExtent = yExtent;
    }
    if (zExtent >= xExtent && zExtent >= yExtent) {
        maxExtent = zExtent;
    }
    scaleX = 2.0 /maxExtent;
    shiftX = minX + (xExtent/ 2.0);
    scaleY = 2.0 / maxExtent;
    shiftY = minY + (yExtent / 2.0);
    scaleZ = 2.0/ maxExtent;
    shiftZ = minZ + (zExtent)/2.0;
    
    //Go through all verticies shift and scale them
    for (size_t i = 0; i < shapes.size(); i++) {
        for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
            shapes[i].mesh.positions[3*v+0] = (shapes[i].mesh.positions[3*v+0] - shiftX) * scaleX;
            assert(shapes[i].mesh.positions[3*v+0] >= -1.0 - epsilon);
            assert(shapes[i].mesh.positions[3*v+0] <= 1.0 + epsilon);
            shapes[i].mesh.positions[3*v+1] = (shapes[i].mesh.positions[3*v+1] - shiftY) * scaleY;
            assert(shapes[i].mesh.positions[3*v+1] >= -1.0 - epsilon);
            assert(shapes[i].mesh.positions[3*v+1] <= 1.0 + epsilon);
            shapes[i].mesh.positions[3*v+2] = (shapes[i].mesh.positions[3*v+2] - shiftZ) * scaleZ;
            assert(shapes[i].mesh.positions[3*v+2] >= -1.0 - epsilon);
            assert(shapes[i].mesh.positions[3*v+2] <= 1.0 + epsilon);
        }
    }
}
Mesh& Mesh::with(Record* record)
{
    this->record = record;
    return *this;
}

void Mesh::draw(glm::mat4 camera)
{
    program.use();
    glBindVertexArray(gVAO);
    program.setUniform("model", record->getModelMat());
    program.setUniform("camera", camera);    
    glDrawElements(GL_TRIANGLES, shapes[0].mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    program.stopUsing();
    
}