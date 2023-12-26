#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>

// GLM Files - Math Library
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Utilities.h"

// To directly pass various lights into the shader
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class Shader {
private:
    int pointLightCount;
    int spotLightCount;

    GLuint  shaderID, uniformProjection, uniformModel, uniformView, uniformEyePosition,
            uniformSpecularIntensity, uniformShininess,
            uniformTexture, uniformDirectionalShadowMap,
            uniformDirectionalLightTransform,
            uniformOmniLightPos, uniformFarPlane;

    GLuint uniformLightMatrices[6];

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

    struct {
        GLuint shadowMap;
        GLuint farPlane;
    }
    uniformOmniShadowMap[MAX_SPOT_LIGHTS + MAX_POINT_LIGHTS];

    void compileShader(const char* vertexCode, const char* fragmentCode);
    void compileShader(const char* vertexCode, const char* fragmentCode, const char* geometryCode);

    void addShader(GLuint program, const char* shaderCode, GLenum shaderType);

    void compileProgram();

public:
    // Constructor
    Shader();

    void createFromString(const char* vertexCode, const char* fragmentCode);
    void createFromFiles(const char* vertexLocation, const char* fragmentLocation);
    void createFromFiles(const char* vertexLocation, const char* fragmentLocation, const char* geometryLocation);

    void validate();

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
    GLuint getOmniLightPosLocation();
    GLuint getFarPlaneLocation();

    void setDirectionalLight(DirectionalLight* dLight);
    void setPointLight(PointLight* pLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
    void setSpotLight(SpotLight* sLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
    void setTexture(GLuint textureUnit);
    void setDirectionalShadowMap(GLuint textureUnit);
    void setDirectionalLightTransform(glm::mat4* lTransform);
    void setLightMatrices(std::vector<glm::mat4> lightMatrices);

    void useShader();
    void cleanShader();

    // Destructor
    ~Shader();
};
