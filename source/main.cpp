//
// seriously modified ZJ Wood January 2015 - conversion to glfw
// inclusion of matrix stack Feb. 2015
// original from Shinjiro Sueda
// October, 2014
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <stdio.h>
#include "GLSL.h"
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr
#include "RenderingHelper.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include "Camera.h"
#include "GameObject.h"
#include "Shader.h"
#include "Material.h"
#include "GameModel.h"
#include <map>

GLFWwindow* window;
using namespace std;
using namespace glm;


static float g_width, g_height;

// Mouse position stuff
double mouseX;
double mouseY;

// CAMERA STUFF
mat4 view;

Camera camera;

//declare a matrix stack
RenderingHelper ModelTrans;

vector<GameObject> worldObjects;
map<string, GameModel> models;
map<string, Material> materials;
Shader phongShader;
MeshLoader loader = MeshLoader();



void initMaterials()
{
    materials.insert(pair<string, Material>(
        "stone",
        Material(
        vec3(10, 10, 10) / 255.0f, // Ambient
        vec3(200, 200, 200) / 255.0f, // Diffuse
        vec3(50, 50, 50) / 255.0f, // Specular
        1.0f // Shininess
    )));
}

void setMaterial(Material material)
{
    glUniform3fv(phongShader.getUniformHandle("UaColor"), 1, glm::value_ptr(material.ambient));
    glUniform3fv(phongShader.getUniformHandle("UdColor"), 1, glm::value_ptr(material.diffuse));
    glUniform3fv(phongShader.getUniformHandle("UsColor"), 1, glm::value_ptr(material.specular));
    glUniform1f(phongShader.getUniformHandle("Ushine"), material.shininess);
}

void setLight(vec3 lightPos)
{
    glUniform3fv(phongShader.getUniformHandle("uLightPos"), 1, glm::value_ptr(lightPos));
}

void initModels()
{
    loader.LoadMeshes({ "bunny.obj" });

    GameModel bunny = GameModel(*loader.getMesh("bunny"), 0.45f);
    bunny.resetTransformation();
    bunny.addMaterial(materials.at("stone"));
    models.insert(pair<string, GameModel>("bunny", bunny));
}

void initObjects()
{
    worldObjects.push_back(GameObject(&models.at("bunny"), OBJECT_SCENERY, vec3(0.0, 0.0, 0.0)));
}

void initGL()
{
    // Set the background color
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    // Enable Z-buffer test
    glEnable(GL_DEPTH_TEST);
    glPointSize(18);

    phongShader = Shader("vert.glsl", "frag.glsl");
    initMaterials();
    initModels();
    initObjects();
    setLight(vec3(0.0, 10.0, 0.0));

    //initialize the modeltrans matrix stack
    ModelTrans.useModelViewMatrix();
    ModelTrans.loadIdentity();

}

void drawGL()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(phongShader.getProgramID());

    glUniformMatrix4fv(phongShader.getUniformHandle("uViewMatrix"), 1, GL_FALSE, glm::value_ptr(camera.get_view_matrix()));
    glUniformMatrix4fv(phongShader.getUniformHandle("uProjMatrix"), 1, GL_FALSE, glm::value_ptr(glm::perspective((float)radians(45.0), (float)g_width / g_height, 0.1f, 100.f)));
    for each (GameObject object in worldObjects)
    {
        GameModel *model = object.getModel();
        Mesh mesh = model->getMesh();
        setMaterial(materials.at("stone"));
        glEnableVertexAttribArray(phongShader.getAttributeHandle("aPosition"));
        glEnableVertexAttribArray(phongShader.getAttributeHandle("aNormal"));

        glBindBuffer(GL_ARRAY_BUFFER, mesh.getPosBufferObject(0));
        glVertexAttribPointer(phongShader.getAttributeHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.getNorBufferObject(0));
        glVertexAttribPointer(phongShader.getAttributeHandle("aNormal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getIdxBufferObject(0));
        glUniformMatrix4fv(phongShader.getUniformHandle("uModelMatrix"), 1, GL_FALSE, glm::value_ptr(object.getTransformation() * model->getTransformationMatrix()));
        glDrawElements(GL_TRIANGLES, mesh.indexCount(0), GL_UNSIGNED_INT, 0);
   }

    GLSL::disableVertexAttribArray(phongShader.getAttributeHandle("aPosition"));
    GLSL::disableVertexAttribArray(phongShader.getAttributeHandle("aNormal"));
}

void reshapeGL(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	g_width = w;
	g_height = h;
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Open a window and create its OpenGL context
    g_width = 1280;
    g_height = 720;
    window = glfwCreateWindow(g_width, g_height, "P3 - shading", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Set camera to be used for callbacks
    Camera::set_debug_camera(shared_ptr<Camera>(&camera));

	glfwSetKeyCallback(window, &Camera::debug_key_callback);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);



	// FOR MOUSE
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, &Camera::debug_mouse_callback);
	glfwSetCursorPos(window, 0.0, 0.0);


	std::cout << " loaded the object " << endl;

	initGL();

	do
	{
        camera.movement();
		drawGL();
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	glfwTerminate();
	return 0;
}
