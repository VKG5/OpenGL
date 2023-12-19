#pragma once

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>

// GLM Files - Math Library
#include <glm/glm.hpp>

#include "ShadowMap.h"

class Light {
protected:
    // The amount of light being reflected from the surface upon hitting
    glm::vec3 colour;
    GLfloat ambientIntensity;
    GLfloat diffuseIntensity;

    glm::mat4 lightProj;

    ShadowMap* shadowMap;

public:
    // Constructor
    Light();

    Light(  GLuint shadowWidth, GLuint shadowHeight,
            GLfloat red, GLfloat green, GLfloat blue,
            GLfloat ambIntensity, GLfloat diffIntensity );

    ShadowMap* getShadowMap() { return shadowMap; }

    // Destructor
    ~Light();
};
