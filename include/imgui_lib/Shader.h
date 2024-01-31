#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>

#include "Utilities.h"

// To directly pass various lights into the shader
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class Shader {
private:
    int pointLightCount;
    int spotLightCount;

    GLuint  shaderID, uniformProjection, uniformModel, uniformView,
            uniformEyePosition, uniformSpecularIntensity, uniformShininess;

    // Creating instance of struct - uniformDirectionalLight
    struct {
        GLuint uniformColour;
        GLuint uniformAmbientIntensity;
        GLuint uniformDiffuseIntensity;

        GLuint uniformDirection;
    } uniformDirectionalLight;

    GLuint uniformPointLightCount;

    struct {
        GLuint uniformColour;
        GLuint uniformAmbientIntensity;
        GLuint uniformDiffuseIntensity;

        GLuint uniformPosition;
        GLuint uniformConstant;
        GLuint uniformLinear;
        GLuint uniformExponent;
    } uniformPointLight[MAX_POINT_LIGHTS];

    GLuint uniformSpotLightCount;

    struct {
        GLuint uniformColour;
        GLuint uniformAmbientIntensity;
        GLuint uniformDiffuseIntensity;

        GLuint uniformPosition;
        GLuint uniformConstant;
        GLuint uniformLinear;
        GLuint uniformExponent;

        GLuint uniformDirection;
        GLuint uniformEdge;
    } uniformSpotLight[MAX_SPOT_LIGHTS];

    // For switching between different shading models
    GLuint uniformshadingModel;

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
    GLuint getUniformshadingModel();
    GLuint getShaderID();

    void setDirectionalLight(DirectionalLight* dLight);
    void setPointLight(PointLight* pLight, unsigned int lightCount);
    void setSpotLight(SpotLight* sLight, unsigned int lightCount);

    void useShader();
    void cleanShader();

    // Destructor
    ~Shader();
};