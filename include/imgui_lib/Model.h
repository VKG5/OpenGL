#pragma once

// General libraries
#include <iostream>
#include <vector>
#include <string>

// ASSIMP File Importer
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Custom libraries
#include "Mesh.h"
#include "Texture.h"

class Model {
private:
    std::vector<Mesh*> meshList;
    std::vector<Texture*> textureList;
    std::vector<unsigned int> meshToTex;

    // To load children data
    void loadNode(aiNode *node, const aiScene *scene);
    void loadMesh(aiMesh *mesh, const aiScene *scene);
    void loadMaterials(const aiScene *scene);
    void loadMap(aiMaterial* material, aiTextureType textureType, int textureIndex);

public:
    // Constructor
    Model();

    void loadModel(const std::string filePath);
    void renderModel();
    void clearModel();

    // Destructor
    ~Model();
};
