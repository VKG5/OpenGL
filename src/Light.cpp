#include "Light.h"

Light::Light() {
    // Ambient Light
    colour = glm::vec3(1.0f, 1.0f, 1.0f);
    ambientIntensity = 1.0f;

    // Diffuse Light
    direction = glm::vec3(0.0f, -1.0f, 0.0f);
    diffuseIntensity = 0.0f;
}

Light::Light(   GLfloat red, GLfloat green, GLfloat blue, GLfloat ambIntensity,
                GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat diffIntensity ) {
    // Ambient Light
    colour = glm::vec3(red, green, blue);
    ambientIntensity = ambIntensity;

    // Diffuse Light
    direction = glm::vec3(xDir, yDir, zDir);
    diffuseIntensity = diffIntensity;
}

void Light::useLight(   GLuint ambientIntensityLocation, GLuint ambientColourLocation,
                        GLuint diffuseIntensityLocation, GLuint directionLocation  ) {
    // Ambient Light
    glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
    glUniform1f(ambientIntensityLocation, ambientIntensity);

    // Diffuse Light
    glUniform3f(directionLocation, direction.x, direction.y, direction.z);
    glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

Light::~Light() {
}
