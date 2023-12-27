#include "Skybox.h"

Skybox::Skybox() {
}

Skybox::Skybox(std::vector<std::string> faceLocations) {
    // Shader setup
    skyShader = new Shader();
    skyShader->createFromFiles("D:/Programs/C++/Yumi/src/Shaders/skybox.vert", "D:/Programs/C++/Yumi/src/Shaders/skybox.frag");

    uniformProjection = skyShader->getProjectionLocation();
    uniformView = skyShader->getViewLocation();

    // Texture setup
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, bitDepth;

    for(size_t i = 0; i < 6; i++) {
        unsigned char *texData = stbi_load(faceLocations[i].c_str(), &width, &height, &bitDepth, 0);
        if(!texData) {
            printf("Failed to load: %s\n", faceLocations[i].c_str());
            return;
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);

        // Removing images from memory
        stbi_image_free(texData);
    }

    // For zooming out - Minify
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // For zooming in - Magnify
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Setting parameter values
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // For z coordinate
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Mesh Setup
    unsigned int skyboxIndices[] = {
        // Front
        0, 1, 2,
        2, 1, 3,

        // Right
        2, 3, 5,
        5, 3, 7,

        // Back
        5, 7, 4,
        4, 7, 6,

        // Left
        4, 6, 0,
        0, 6, 1,

        // Top
        4, 0, 5,
        5, 0, 2,

        // Bottom
        1, 6, 3,
        3, 6, 7
    };

    // Placing the images based off of camera values instead of UV
    float skyboxVertices[] = {
        // Back corners
        -1.0f, 1.0f, -1.0f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,   0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, -1.0f,     0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,

        // Front corners
        -1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f,     0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
    };

    skyMesh = new Mesh();
    skyMesh->createMesh(skyboxVertices, skyboxIndices, 64, 36);
}

void Skybox::drawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
    // Removing the Transform values from mat4 by converting into mat3
    // Keeping only Translation and Rotation
    viewMatrix = glm::mat4(glm::mat3(viewMatrix));

    // Turning off GL Depth Mask so that the skybox doesn't overlap anything objects
    glDepthMask(GL_FALSE);

    // Main Skybox code
    skyShader->useShader();

    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    skyShader->validate();

    skyMesh->renderMesh();

    // Turning it back on, so that we can draw our objects in the scene properly (Without artifacts)
    // To prevent weird overlapping of vertices/edges/faces
    glDepthMask(GL_TRUE);
}

Skybox::~Skybox() {
}
