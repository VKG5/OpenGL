#include "DirectionalLight.h"

// Calling Super Contructor
DirectionalLight::DirectionalLight() : Light() {
    // Diffuse Light
    direction = glm::vec3(0.0f, -1.0f, 0.0f);
}

DirectionalLight::DirectionalLight( GLfloat red, GLfloat green, GLfloat blue,
                                    GLfloat ambIntensity, GLfloat diffIntensity,
                                    GLfloat xDir, GLfloat yDir, GLfloat zDir ) : Light(red, green, blue, ambIntensity, diffIntensity) {
    // Diffuse Light
    direction = glm::vec3(xDir, yDir, zDir);
}

void DirectionalLight::useLight( GLuint ambientIntensityLocation, GLuint ambientColourLocation,
                                 GLuint diffuseIntensityLocation, GLuint directionLocation  ) {
    // Ambient Light
    glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
    glUniform1f(ambientIntensityLocation, ambientIntensity);

    // Diffuse Light
    glUniform3f(directionLocation, direction.x, direction.y, direction.z);
    glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

DirectionalLight::~DirectionalLight() {
}
