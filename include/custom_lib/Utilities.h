#pragma once
// Always include GLFW after GLAD - Core Libraries
#include <glad.h>

// GLM Files - Math Library
#include <glm/glm.hpp>

// Common Values
const int MAX_POINT_LIGHTS = 3;

// Averaging Normals for Phong Shading
void calcAverageNormals(unsigned int* indices, unsigned int indexCount, GLfloat* vertices, unsigned int vertexCount, unsigned int vLength, unsigned int normalOffset);
