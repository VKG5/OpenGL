#include "Material.h"

Material::Material() {
    specularIntensity = 0.0f;
    shininess = 0.5f;
}


Material::Material(GLfloat specIntensity, GLfloat shine) {
    specularIntensity = specIntensity;
    shininess = shine;

}

void Material::useMaterial(GLuint specularIntensityLocation, GLuint shininessLocation) {
    // Specular Highlights
    glUniform1f(specularIntensityLocation, specularIntensity);
    glUniform1f(shininessLocation, shininess);
}

Material::~Material() {
}
