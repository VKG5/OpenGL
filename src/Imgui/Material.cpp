#include "Material.h"

Material::Material() {
    specularIntensity = 0.0f;
    shininess = 0.5f;
    roughness = 0.5f;
}


Material::Material(GLfloat specIntensity, GLfloat shine, GLfloat rough) {
    specularIntensity = specIntensity;
    shininess = shine;
    roughness = rough;
}

void Material::useMaterial(GLuint specularIntensityLocation, GLuint shininessLocation, GLuint roughnessLocation) {
    // Specular Highlights
    glUniform1f(specularIntensityLocation, specularIntensity);
    glUniform1f(shininessLocation, shininess);
    glUniform1f(roughnessLocation, roughness);
}

Material::~Material() {
}
