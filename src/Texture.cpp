#include "Texture.h"

Texture::Texture() {
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    filePath = "";
}

Texture::Texture(const char* fileLoc) {
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    filePath = fileLoc;
}

// Load Textures without Alpha Channel
bool Texture::loadTexture() {
    unsigned char *texData = stbi_load(filePath, &width, &height, &bitDepth, 0);
    if(!texData) {
        printf("Failed to load: %s\n", filePath);
        return false;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Setting parameter values
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // For zooming out - Minify
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // For zooming in - Magnify
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Change the RGB type based off of your image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbinding Texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // We have already copied the data
    stbi_image_free(texData);

    return true;
}

// Load Textures with Alpha Channel
bool Texture::loadTextureA() {
    unsigned char *texData = stbi_load(filePath, &width, &height, &bitDepth, 0);
    if(!texData) {
        printf("Failed to load: %s\n", filePath);
        return false;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Setting parameter values
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // For zooming out - Minify
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // For zooming in - Magnify
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Change the RGB type based off of your image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbinding Texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // We have already copied the data
    stbi_image_free(texData);

    return true;
}

void Texture::useTexture() {
    // Texture Unit
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::cleanTexture() {
    glDeleteTextures(1, &textureID);
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    filePath = "";
}

Texture::~Texture() {
    cleanTexture();
}
