#pragma once

// General libraries
#include <iostream>
#include <vector>
#include <string>

// GLM Files - Math Library
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

// ASSIMP File Importer
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Custom libraries
#include "Mesh.h"
#include "Texture.h"
#include "Utilities.h"

class Model {
private:
    std::vector<Mesh*> meshList;
    std::vector<Texture*> textureList;
    std::vector<unsigned int> meshToTex;

    // Local Transforms
    glm::vec3 localPosition;
    glm::vec3 localRotation;
    glm::vec3 localScale;

    // Children pointer
    std::vector<Model*> children;

    // Parent pointer
    Model* parent;

    // Transform Matrix without transformations
    glm::mat4 initialTransform;

    // Transform Matrix with transformations
    glm::mat4 accumulateTransform;

    // To load children data
    void loadNode(aiNode *node, const aiScene *scene);
    void loadMesh(aiMesh *mesh, const aiScene *scene);
    void loadMaterials(const aiScene *scene);
    void loadMap(aiMaterial* material, aiTextureType textureType, int textureIndex);

public:
    // Constructor
    Model();

    void loadModel(const std::string& filePath);
    void renderModel(const GLuint& uniformModel);
    void clearModel();

    // Getters
    glm::mat4 getInitialTransformMatrix() { return initialTransform; }
    glm::mat4 getAccumulateTransformMatrix() { return accumulateTransform; }
    glm::vec3 getPosition() { return glm::vec3(accumulateTransform[3]); }
    Model* getParent() { return parent; }
    const std::vector<Model*>& getChildren() const { return children; }

    // Recursive function to calculate chain length
    GLuint calculateChainLength();

    // Function to clear the list of children
    void clearChildren();

    // Setters
    void setLocalTransforms(glm::vec3 translate = glm::vec3(0.0f), glm::vec3 rotate = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f));
    void setInitialTransformMatrix();
    void attachChild(Model* childModel);
    void attachParent(Model* parentModel);
    void resetTransform(glm::mat4& localTransform = glm::mat4(1.0f));
    void updateTransform(glm::mat4& parentTransform = glm::mat4(1.0f));

    // TRS
    void updateTranslation(glm::vec3& offset);
    void updateRotation(GLfloat angle, glm::vec3& axis, bool rads);
    void updateScale(glm::vec3& scale);

    // Destructor
    ~Model();
};
