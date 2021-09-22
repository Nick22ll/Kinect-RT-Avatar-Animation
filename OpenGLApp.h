#pragma once

#include <chrono>
#include <thread>
#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <Kinect.h>
#include<Windows.h>
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "Sensor.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"
#include "glfw3.h"
#include"Texture.h"
using namespace std::chrono;
using namespace std;
typedef steady_clock Clock;

//size of the window
#define SCRWIDTH 1920
#define SCRHEIGHT 1080

static const GLfloat cube_vertices[] = {
    //vertici di un cubo in coordinate OpenGL partendo dal vertice in alto a sinistra della faccia sulle z negative(senso orario)
    
    //COORDINATES               COLOR RGB               TEXTURE COORD.        NORMALS
    //faccia frontale
     -0.5f, 0.5f, 0.5f,      1.0f, 0.0f, 0.0f,        0.0f, 1.0f,     0.0f,0.0f,1.0f,
     0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 0.0f,        1.0f, 1.0f,     0.0f,0.0f,1.0f,
     0.5f, -0.5f, 0.5f,      1.0f, 0.0f, 0.0f,        1.0f, 0.0f,     0.0f,0.0f,1.0f,
     -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f,        0.0f, 0.0f,     0.0f,0.0f,1.0f,
            
    //faccia posteriore
    -0.5f, 0.5f, -0.5f,      1.0f, 0.0f, 0.0f,        0.0f, 1.0f,     0.0f,0.0f,-1.0f,
    0.5f, 0.5f, -0.5f,       1.0f, 0.0f, 0.0f,        1.0f, 1.0f,     0.0f,0.0f,-1.0f,
    0.5f, -0.5f, -0.5f,      1.0f, 0.0f, 0.0f,        1.0f, 0.0f,     0.0f,0.0f,-1.0f,
    -0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f,        0.0f, 0.0f,     0.0f,0.0f,-1.0f,

    //faccia sinistra
    -0.5f, 0.5f, 0.5f,      1.0f, 0.0f, 0.0f,        0.0f, 1.0f,     -1.0f,0.0f,0.0f,
    -0.5f, 0.5f, -0.5f,     1.0f, 0.0f, 0.0f,        1.0f, 1.0f,     -1.0f,0.0f,0.0f,
    -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,        1.0f, 0.0f,     -1.0f,0.0f,0.0f,
    -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f,        0.0f, 0.0f,     -1.0f,0.0f,0.0f,

    //faccia superiore
    -0.5f, 0.5f, 0.5f,      0.6f, 0.0f, 0.4f,        0.0f, 1.0f,     0.0f,1.0f,0.0f,
     0.5f, 0.5f, 0.5f,      1.0f, 0.0f, 0.5f,        1.0f, 1.0f,     0.0f,1.0f,0.0f,
     0.5f, 0.5f, -0.5f,     0.0f, 1.0f, 0.0f,        1.0f, 0.0f,     0.0f,1.0f,0.0f,
    -0.5f, 0.5f, -0.5f,     1.0f, 0.0f, 0.0f,        0.0f, 0.0f,     0.0f,1.0f,0.0f,

    //faccia destra
    0.5f, 0.5f, -0.5f,       0.0f, 1.0f, 0.0f,        0.0f, 1.0f,     1.0f,0.0f,0.0f,
    0.5f, 0.5f, 0.5f,        1.0f, 0.0f, 0.5f,        1.0f, 1.0f,     1.0f,0.0f,0.0f,
    0.5f, -0.5f, 0.5f,       1.0f, 1.0f, 0.0f,        1.0f, 0.0f,     1.0f,0.0f,0.0f,
    0.5f, -0.5f, -0.5f,      0.0f, 0.0f, 1.0f,        0.0f, 0.0f,     1.0f,0.0f,0.0f,

    //faccia inferiore
     -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 1.0f,        0.0f, 1.0f,     0.0f,-1.0f,0.0f,
     0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 0.0f,        1.0f, 1.0f,     0.0f,-1.0f,0.0f,
     0.5f, -0.5f, -0.5f,      0.0f, 0.0f, 1.0f,       1.0f, 0.0f,     0.0f,-1.0f,0.0f,
     -0.5f, -0.5f, -0.5f,     0.5f, 0.5f, 0.0f,       0.0f, 0.0f,     0.0f,-1.0f,0.0f
};


static const GLfloat pyramid_vertices[] =
{ //     COORDINATES     /        COLORS          /    TexCoord   /        NORMALS       //
    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side
    -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
     0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
     0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side

    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
    -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,     -0.8f, 0.5f,  0.0f, // Left Side

    -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
     0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.0f, 0.5f, -0.8f, // Non-facing side

     0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
     0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.8f, 0.5f,  0.0f, // Right side

     0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.0f, 0.5f,  0.8f  // Facing side
};

// Indices for vertices order
static const GLuint pyramid_indices[] =
{
    0, 1, 2, // Bottom side
    0, 2, 3, // Bottom side
    4, 6, 5, // Left side
    7, 9, 8, // Non-facing side
    10, 12, 11, // Right side
    13, 15, 14 // Facing side
};


static const GLfloat lightVertices[] =
{ //     COORDINATES     //
    -0.1f, -0.1f,  0.1f,
    -0.1f, -0.1f, -0.1f,
     0.1f, -0.1f, -0.1f,
     0.1f, -0.1f,  0.1f,
    -0.1f,  0.1f,  0.1f,
    -0.1f,  0.1f, -0.1f,
     0.1f,  0.1f, -0.1f,
     0.1f,  0.1f,  0.1f
};

static const GLuint lightIndices[] =
{
    0, 1, 2,
    0, 2, 3,
    0, 4, 7,
    0, 7, 3,
    3, 7, 6,
    3, 6, 2,
    2, 6, 5,
    2, 5, 1,
    1, 5, 4,
    1, 4, 0,
    4, 5, 6,
    4, 6, 7
};


//some useful typedefs for explicit type sizes
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;


class OpenGLApp
{
public:
    OpenGLApp();
    void Run();
    void Shutdown();
    int setWindowSize(int w = -1, int h = -1);
    int eventHandler(SDL_Event& event);
    void drawGLfloat(GLfloat* array, Texture texture);
    void drawSkeleton(glm::mat4 transformation = glm::mat4(1.0f));
    void drawAvatarSkeleton(ComplexModel avatar, glm::mat4 transformation = glm::mat4(1.0f));
    void drawLight();
    void drawFromVector(vector<float> vertices);
    void drawComplexModel(ComplexModel complexModel, Texture& texture, glm::mat4 transformation=glm::mat4(1.0f));
    void drawComplexModel(ComplexModel complexModel, glm::mat4 transformation=glm::mat4(1.0f));
    void drawCubeSkeleton(glm::mat4 transformation = glm::mat4(1.0f));
private:
    vector<Skeleton> skeletons;
    Sensor* kinect = nullptr;
    SDL_Window* window = nullptr;
    int windowWidth = SCRWIDTH, windowHeight = SCRHEIGHT;
    steady_clock::time_point lastSkeletonRead = Clock::now();
    Camera camera;
    //Shader declaration
    Shader DefaultShader, LightShader, AlignShader, NoTextureShader;

    // Create reference containers for the Vertex Array Object and the Vertex Buffer Object
    VAO defaultVAO, lightVAO, modelVAO;
    VBO defaultVBO, lightVBO, modelVBO;
    EBO defaultEBO, lightEBO, modelEBO;
    float rotation = 1.0f;
    Texture defaultTexture, floorTexture, avatarTexture, woodTexture;
    GLuint windowWidthID, windowHeightID;
    int SDL_initialization();
};




