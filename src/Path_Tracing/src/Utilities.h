#ifndef COMMONVALS
#define COMMONVALS

// Always include GLFW after GLAD - Core Libraries
//#include <glad.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM Files - Math Library
#include <glm/glm.hpp>

// Image Handling
#include <stb_image.h>
#include "stb_image_write.h"

#define minPCG 0
#define maxPCG 1000
#define MAX_OBJECT_COUNT 64
#define MAX_LIGHT_COUNT 4
#define CDS_FULLSCREEN 4

// For procedural content generation
#include <random>

// Setting a number for the seed to get same results every time
int seed = 692360;

// Seeding the generator
std::mt19937 gen(seed);

//  Vertex data for a cube, I am not using indexed draws for ease of writing
const GLfloat vertices[] = {
	-1.0F, -1.0F, 0.0F,
	1.0F, -1.0F, 0.0F,
	1.0F, 1.0F, 0.0F,
	1.0F, 1.0F, 0.0F,
	-1.0F, 1.0F, 0.0F,
	-1.0F, -1.0F, 0.0F
};

// UV data for a cube
const GLfloat UVs[] = {
	0.0F, 0.0F,
	1.0F, 0.0F,
	1.0F, 1.0F,
	1.0F, 1.0F,
	0.0F, 1.0F,
	0.0F, 0.0F,
};

// Defining the dimensions of the screen
int screenWidth = 1920, screenHeight = 1080;

#endif
