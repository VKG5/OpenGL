#pragma once

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>

// This class contains the material properties
class Material {
private:
    GLfloat specularIntensity;
    GLfloat shininess;
    GLfloat roughness;

public:
    // Constructor
    Material();

    Material(GLfloat specIntensity, GLfloat shine, GLfloat rough);

    void useMaterial(GLuint specularIntensityLocation, GLuint shininessLocation, GLuint roughnessLocation);

    // Destructor
    ~Material();
};
