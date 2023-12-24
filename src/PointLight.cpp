#include "PointLight.h"

PointLight::PointLight() : Light() {
    // Position of the light
    position = glm::vec3(0.0f, 1.0f, 0.0f);

    // Attenuation Values
    // L / (ax^2 + bx + c)
    constant = 1.0f;
    linear = 0.0f;
    exponent = 0.0f;
}

PointLight::PointLight( GLuint shadowWidth, GLuint shadowHeight,
                        GLfloat near, GLfloat far,
                        GLfloat red, GLfloat green, GLfloat blue,
                        GLfloat ambIntensity, GLfloat diffIntensity,
                        GLfloat xPos, GLfloat yPos, GLfloat zPos,
                        GLfloat cons, GLfloat lin, GLfloat exp ) : Light(shadowWidth, shadowHeight, red, green, blue, ambIntensity, diffIntensity) {
    // Position of the light
    position = glm::vec3(xPos, yPos, zPos);

    // Attenuation Values
    constant = cons;
    linear = lin;
    exponent = exp;

    farPlane = far;

    // Getting the aspect ratio
    float aspect = (float)shadowWidth / (float)shadowHeight;

    lightProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

    shadowMap = new OmniShadowMap();
    shadowMap->init(shadowWidth, shadowHeight);
}

void PointLight::useLight(  GLuint ambientIntensityLocation, GLuint ambientColourLocation,
                            GLuint diffuseIntensityLocation, GLuint positionLocation,
                            GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation  ) {
    // Ambient Light
    glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
    glUniform1f(ambientIntensityLocation, ambientIntensity);

    // Diffuse Light
    glUniform1f(diffuseIntensityLocation, diffuseIntensity);

    // Point Light
    glUniform3f(positionLocation, position.x, position.y, position.z);

    // Attenuation Factor
    glUniform1f(constantLocation, constant);
    glUniform1f(linearLocation, linear);
    glUniform1f(exponentLocation, exponent);
}

std::vector<glm::mat4> PointLight::calculateLightTransform() {
    std::vector<glm::mat4> lightMatrices;

    // Positive X
    lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    // Negative X
    lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

    // Positive Y
    lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    // Negative Y
    lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));

    // Positive Z
    lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    // Negative Z
    lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

    return lightMatrices;
}

GLfloat PointLight::getFarPlane() {
    return farPlane;
}

PointLight::~PointLight() {
}
