#pragma once

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>

// GLM Files - Math Library
#include <glm/glm.hpp>

class Light {
private:
    // The amount of light being reflected from the surface upon hitting
    glm::vec3 colour;
    GLfloat ambientIntensity;

public:
    // Constructor
    Light();

    Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambIntensity);

    // Function override for different types of light
    void useLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation);

    // Destructor
    ~Light();
};
