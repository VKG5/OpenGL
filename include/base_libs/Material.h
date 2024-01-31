#pragma once

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>

// This class contains the material properties
class Material {
private:
    GLfloat specularIntensity;
    GLfloat shininess;

public:
    // Constructor
    Material();

    Material(GLfloat specIntensity, GLfloat shine);

    void useMaterial(GLuint specularIntensityLocation, GLuint shininessLocation);

    // Destructor
    ~Material();
};
