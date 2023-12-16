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

    glm::vec3 direction;
    GLfloat diffuseIntensity;

public:
    // Constructor
    Light();

    Light(  GLfloat red, GLfloat green, GLfloat blue, GLfloat ambIntensity,
            GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat diffIntensity );

    // Function override for different types of light
    void useLight(  GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
                    GLfloat diffuseIntensityLocation, GLfloat directionLocation  );

    // Destructor
    ~Light();
};
