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
    // RGB - 3 channels
    if(bitDepth == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
    }

    // RGBA - 4 channels
    else if(bitDepth == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    }

    else {
            printf("The texture has %i channels, unable to load!", bitDepth);
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbinding Texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // We have already copied the data
    stbi_image_free(texData);

    return true;
}

unsigned char* Texture::generateNoise() {
    // Assuming you want RGB values, adjust as needed
    int channels = bitDepth;
    unsigned char* texData = new unsigned char[width * height * channels];

    for (int i = 0; i < width * height * channels; ++i) {
        texData[i] = rand() % 256; // Generate random values between 0 and 255
    }

    return texData;
}

// Generating a random texture for use with roughness map
bool Texture::generateRandomTexture(GLuint w, GLuint h, GLuint d) {
    width = w;
    height = h;
    bitDepth = d;

    unsigned char *texData = generateNoise();

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
    delete[] texData;

    // Debugging
    printf("Generated Noise Texture successfully!");

    return true;
}

// Default Texture Unit
void Texture::useTexture() {
    // Texture Unit
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

// Manually setting the Texture Unit
void Texture::useTexture(int textureUnit) {
    // Texture Unit - Incrementing it based on an integer
    glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(textureUnit));
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
