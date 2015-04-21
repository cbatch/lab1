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
#include "stb_easy_font.h"

const float GENERATE_TIME = 1.0f;
const float PLANE_SCALE = 10.0f;
const float VELOCITY = PLANE_SCALE / 5.0f;
const int COIN_COUNT = 30;

GLFWwindow* window;
using namespace std;
using namespace glm;


static float g_width, g_height;

// Mouse position stuff
double mouseX;
double mouseY;

int collected_coins = 0;

// CAMERA STUFF
mat4 view;

Camera camera;

//declare a matrix stack
RenderingHelper ModelTrans;

vector<GameObject> worldObjects;
deque<GameObject> inside_butterfly_net;
map<string, GameModel> models;
map<string, Material> materials;
Shader phongShader;
//Shader textShader;
MeshLoader loader = MeshLoader();

float lastTime;

float rand_FloatRange(float a, float b)
{
    return ((b - a)*((float)rand() / RAND_MAX)) + a;
}

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

    materials.insert(pair<string, Material>(
        "gold",
        Material(
        vec3(0.12, 0.12, 0.06), // Ambient
        vec3(1.0, 0.8, 0.0), // Diffuse
        vec3(0.4, 0.4, 0.14), // Specular
        200.0f // Shininess
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
    loader.LoadMeshes({ "ground.obj","sphere.obj" });

    GameModel ground = GameModel(*loader.getMesh("ground"));
    ground.resetTransformation();
    ground.addMaterial(materials.at("stone"));
    ground.scale(PLANE_SCALE, 0.2, PLANE_SCALE);
    models.insert(pair<string, GameModel>("ground", ground));

    GameModel coin = GameModel(*loader.getMesh("sphere"), vec3(0.0, 0.0, 0.0), 0.2f);
    coin.resetTransformation();
    coin.addMaterial(materials.at("gold"));
    coin.translate(0.0, 0.45, 0.0);
    coin.scale(0.2, 0.2, 0.2);
    models.insert(pair<string, GameModel>("sphere", coin));
}

void initObjects()
{
    worldObjects.push_back(GameObject(&models.at("ground"), OBJECT_SCENERY, vec3(0.0, 0.0, 0.0)));
    worldObjects.back().velocity = vec3(0.f, 0.f, 0.f);
}

bool objExists(vec3 loc)
{
    for (int i = 0; i < worldObjects.size(); i++)
    {
        if (worldObjects.at(i).getObjectType() == OBJECT_ITEM && glm::distance(worldObjects.at(i).getLocation(), loc) < 1.0)
            return true;
    }
    return false;
}

void generateCoin()
{
    if (worldObjects.size() <= COIN_COUNT)
    {
        vec3 loc = vec3(rand_FloatRange(-(PLANE_SCALE - 1.0), PLANE_SCALE - 1.0), 0.0, rand_FloatRange(-(PLANE_SCALE - 1.0), PLANE_SCALE - 1.0));
        while (objExists(loc))
            loc = vec3(rand_FloatRange(-(PLANE_SCALE - 1.0), PLANE_SCALE - 1.0), 0.0, rand_FloatRange(-(PLANE_SCALE - 1.0), PLANE_SCALE - 1.0));
        worldObjects.push_back(GameObject(&models.at("sphere"), OBJECT_ITEM, loc, vec3(0.0, rand_FloatRange(-(PLANE_SCALE - 1.0), PLANE_SCALE - 1.0), 0.0)));
        worldObjects.back().velocity = vec3(rand_FloatRange(-VELOCITY, VELOCITY), 0.0f, rand_FloatRange(-VELOCITY, VELOCITY));
    }
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

    //textShader = Shader("textvert.glsl", "textfrag.glsl");

    //initialize the modeltrans matrix stack
    ModelTrans.useModelViewMatrix();
    ModelTrans.loadIdentity();

}

void coin_collection_checker()
{
    vec3 butterfly_net = camera.camera_position + normalize(camera.camera_front);
    for (int i = 1; i < worldObjects.size(); i++)
    {
        if (glm::distance(butterfly_net, worldObjects.at(i).getLocation()) < 1.0)
        {
            inside_butterfly_net.push_back(worldObjects.at(i));
            inside_butterfly_net.back().collected_via_butterfly_net();
            worldObjects.erase(worldObjects.begin() + i, worldObjects.begin() + i + 1);
        }
    }
}

void delete_coin()
{
    inside_butterfly_net.erase(inside_butterfly_net.begin(), inside_butterfly_net.begin() + 1);
    collected_coins++;
}

void collision_checker()
{
    for (int i = 0; i < worldObjects.size(); i++)
    {
        for (int j = i + 1; j < worldObjects.size(); j++)
        {
			if (glm::distance(worldObjects.at(i).getLocation(), worldObjects.at(j).getLocation()) < worldObjects.at(j).getModel()->getCollisionRadius() + worldObjects.at(i).getModel()->getCollisionRadius())
            {
                worldObjects.at(i).velocity *= -1;
                worldObjects.at(j).velocity *= -1;
            }
        }
    }
}

void print_string(float x, float y, char *text, float r, float g, float b)
{
    static char buffer[99999]; // ~500 chars
    int num_quads;
    num_quads = stb_easy_font_print(x, y, text, NULL, buffer, sizeof(buffer));
    glColor3f(r, g, b);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void drawGL()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(phongShader.getProgramID());

    if (glfwGetTime() - lastTime > GENERATE_TIME)
    {
        lastTime = glfwGetTime();
        generateCoin();
    }

	bool hasMoved = false;

    glUniformMatrix4fv(phongShader.getUniformHandle("uViewMatrix"), 1, GL_FALSE, glm::value_ptr(camera.get_view_matrix()));
    glUniformMatrix4fv(phongShader.getUniformHandle("uProjMatrix"), 1, GL_FALSE, glm::value_ptr(glm::perspective((float)radians(45.0), (float)g_width / g_height, 0.1f, 100.f)));
    for (int i = 0; i < worldObjects.size(); i++)// GameObject object in worldObjects)
    {
        GameModel *model = worldObjects.at(i).getModel();// object.getModel();
        Mesh mesh = model->getMesh();
        setMaterial(model->getMaterial(0));
        glEnableVertexAttribArray(phongShader.getAttributeHandle("aPosition"));
        glEnableVertexAttribArray(phongShader.getAttributeHandle("aNormal"));

        glBindBuffer(GL_ARRAY_BUFFER, mesh.getPosBufferObject(0));
        glVertexAttribPointer(phongShader.getAttributeHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
        glVertexAttribPointer(phongShader.getAttributeHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.getNorBufferObject(0));
        glVertexAttribPointer(phongShader.getAttributeHandle("aNormal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getIdxBufferObject(0));
        glUniformMatrix4fv(phongShader.getUniformHandle("uModelMatrix"), 1, GL_FALSE, glm::value_ptr(worldObjects.at(i).getTransformation() * model->getTransformationMatrix()));
        glDrawElements(GL_TRIANGLES, mesh.indexCount(0), GL_UNSIGNED_INT, 0);

		if (worldObjects.at(i).step(delete_coin))
			hasMoved = true;
   }

	if (hasMoved)
	{
		collision_checker();
	}
	coin_collection_checker();

    for (int i = 0; i < inside_butterfly_net.size(); i++)// GameObject object in worldObjects)
    {
        GameModel *model = inside_butterfly_net.at(i).getModel();// object.getModel();
        Mesh mesh = model->getMesh();
        //glEnableVertexAttribArray(phongShader.getAttributeHandle("aPosition"));
        //glEnableVertexAttribArray(phongShader.getAttributeHandle("aNormal"));

        //glBindBuffer(GL_ARRAY_BUFFER, mesh.getPosBufferObject(0));
        //glVertexAttribPointer(phongShader.getAttributeHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
        //glBindBuffer(GL_ARRAY_BUFFER, mesh.getNorBufferObject(0));
        //glVertexAttribPointer(phongShader.getAttributeHandle("aNormal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getIdxBufferObject(0));
        glUniformMatrix4fv(phongShader.getUniformHandle("uModelMatrix"), 1, GL_FALSE, glm::value_ptr(inside_butterfly_net.at(i).getTransformation() * model->getTransformationMatrix()));
        glDrawElements(GL_TRIANGLES, mesh.indexCount(0), GL_UNSIGNED_INT, 0);

		inside_butterfly_net.at(i).step(delete_coin);
    }

    GLSL::disableVertexAttribArray(phongShader.getAttributeHandle("aPosition"));
    GLSL::disableVertexAttribArray(phongShader.getAttributeHandle("aNormal"));

    /*static char fontBuffer[99999];
    int num_quads = stb_easy_font_print(0, 0, "H", NULL, fontBuffer, sizeof(fontBuffer));
    cout << num_quads << endl;
    vector<float> fontPos;
    for (int i = 0; i < num_quads; i++)
    {
        fontPos.push_back((float) fontBuffer[i * 4]);
        cout << (float) fontBuffer[i] << endl;
    }
    GLuint temp = 0;
    glGenBuffers(1, &temp);
    glBindBuffer(GL_ARRAY_BUFFER, temp);
    glBufferData(GL_ARRAY_BUFFER, fontPos.size()*sizeof(float), &fontPos[0], GL_STATIC_DRAW);

    glUseProgram(textShader.getProgramID());
    glEnableVertexAttribArray(textShader.getAttributeHandle("aPosition"));
    glVertexAttribPointer(textShader.getAttributeHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glUniformMatrix4fv(textShader.getUniformHandle("uModelMatrix"), 1, GL_FALSE, value_ptr(glm::scale(vec3(.01))));
    glDrawElements(GL_TRIANGLES, num_quads, GL_UNSIGNED_INT, 0);*/
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

	// Cull back faces
	glCullFace(GL_BACK);

    // Set camera to be used for callbacks
    Camera::set_debug_camera(shared_ptr<Camera>(&camera));

	glfwSetKeyCallback(window, &Camera::debug_key_callback);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);



	// FOR MOUSE
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, &Camera::debug_mouse_callback);
	glfwSetCursorPos(window, 0.0, 0.0);

	initGL();
    lastTime = glfwGetTime();

    double renderTime;
	double lastTime = 0.0;
	int i = 0;

	do
	{
        renderTime = glfwGetTime();
        camera.movement();
		drawGL();
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		renderTime = glfwGetTime() - renderTime;
		if (!(i % 50))
		{
			cout << "Frame Rate: " << 1 / (renderTime * 0.9 + lastTime * 0.1) << endl;
			cout << "Collected Coins: " << collected_coins << endl;
			cout << "Coins on Screen: " << worldObjects.size() - 1 + inside_butterfly_net.size() << endl << endl;
		}
		i++;
		lastTime = renderTime;

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
    && glfwWindowShouldClose(window) == 0);

	glfwTerminate();
	return 0;
}
