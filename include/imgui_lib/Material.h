#pragma once

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>

// This class contains the material properties
class Material {
private:
    GLfloat specularIntensity;
    GLfloat shininess;
    GLfloat metalness;

public:
    // Constructor
    Material();

    Material(GLfloat specIntensity, GLfloat shine, GLfloat metal);

    void useMaterial(GLuint specularIntensityLocation, GLuint shininessLocation, GLuint metalnessLocation);

    // Setter
    void setMaterialParamters(GLfloat specularIntensity, GLfloat shininess, GLfloat metal);

    // Destructor
    ~Material();
};
