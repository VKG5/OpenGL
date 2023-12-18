#pragma once

#include <stdio.h>

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>

class ShadowMap
{
protected:
    GLuint FBO, shadowMap;
    GLuint shadowWidth, shadowHeight;

public:
    // Constructor
    ShadowMap();

    virtual bool init(GLuint width, GLuint height);

    virtual void write();

    virtual void read(GLenum textureUnit);

    GLuint getShadowWidth() { return shadowWidth; }
    GLuint getShadowHeight() { return shadowHeight; }

    // Destructor
    ~ShadowMap();
};
