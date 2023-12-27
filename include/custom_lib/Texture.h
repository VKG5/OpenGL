#pragma once

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>

#include "Utilities.h"

class Texture {
private:
    GLuint textureID;
    int width, height, bitDepth;

    const char* filePath;

public:
    // Constructor
    Texture();

    Texture(const char* fileLoc);

    bool loadTexture();
    bool loadTextureA();
    void useTexture();
    void cleanTexture();

    // Destructor
    ~Texture();
};
