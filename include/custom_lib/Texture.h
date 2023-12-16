#pragma once

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>

// Image Handling
#include <stb_image.h>

class Texture {
private:
    GLuint textureID;
    int width, height, bitDepth;

    char* filePath;

public:
    // Constructor
    Texture();

    Texture(char* fileLoc);

    void loadTexture();
    void useTexture();
    void cleanTexture();

    // Destructor
    ~Texture();
};
