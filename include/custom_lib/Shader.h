#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>

// To directly pass various lights into the shader
#include "DirectionalLight.h"
#include "PointLight.h"

class Shader {
private:
    int pointLightCount;

    GLuint  shaderID, uniformProjection, uniformModel, uniformView,
            uniformEyePosition, uniformSpecularIntensity, uniformShininess;

    // Creating instance of struct - uniformDirectionalLight
    struct {
        GLuint uniformColour;
        GLuint uniformAmbientIntensity;
        GLuint uniformDiffuseIntensity;

        GLuint uniformDirection;
    } uniformDirectionalLight;

    void compileShader(const char* vertexCode, const char* fragmentCode);
    void addShader(GLuint program, const char* shaderCode, GLenum shaderType);

public:
    // Constructor
    Shader();

    void createFromString(const char* vertexCode, const char* fragmentCode);
    void createFromFiles(const char* vertexLocation, const char* fragmentLocation);

    std::string readFile(const char* filePath);

    GLuint getProjectionLocation();
    GLuint getModelLocation();
    GLuint getViewLocation();
    GLuint getAmbientIntensityLocation();
    GLuint getAmbientColourLocation();
    GLuint getDiffuseIntensityLocation();
    GLuint getDiffuseDirectionLocation();
    GLuint getSpecularIntensityLocation();
    GLuint getShininessLocation();
    GLuint getEyePositionLocation();

    void setDirectionalLight(DirectionalLight* dLight);

    void useShader();
    void cleanShader();

    // Destructor
    ~Shader();
};
