#pragma once

#include <vector>
#include <string>

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>

// GLM Files - Math Library
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Custom Headers
#include "Utilities.h"
#include "Mesh.h"
#include "Shader.h"

class Skybox {
private:
    Mesh* skyMesh;
    Shader* skyShader;

    GLuint textureID;
    GLuint uniformProjection, uniformView;

public:
    // Constructor
    Skybox();

    Skybox(std::vector<std::string> faceLocations, bool rgba);

    void drawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

    // Destructor
    ~Skybox();
};
