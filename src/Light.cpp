#include "Light.h"

Light::Light() {
    // Ambient Light
    colour = glm::vec3(1.0f, 1.0f, 1.0f);
    ambientIntensity = 1.0f;

    // Diffuse Light
    diffuseIntensity = 0.0f;
}

Light::Light(   GLuint shadowWidth, GLuint shadowHeight,
                GLfloat red, GLfloat green, GLfloat blue,
                GLfloat ambIntensity, GLfloat diffIntensity ) {
    // Ambient Light
    colour = glm::vec3(red, green, blue);
    ambientIntensity = ambIntensity;

    // Diffuse Light
    diffuseIntensity = diffIntensity;

    // Creating a new shadow map
    shadowMap = new ShadowMap();
    shadowMap->init(shadowWidth, shadowHeight);
}

Light::~Light() {
}
