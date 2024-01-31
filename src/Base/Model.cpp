#include "Model.h"

Model::Model() {
}

void Model::renderModel() {
    for(size_t i = 0; i < meshList.size(); i++) {
        unsigned int materialIndex = meshToTex[i];

        if(materialIndex < textureList.size() && textureList[materialIndex]) {
            textureList[materialIndex]->useTexture();
        }

        meshList[i]->renderMesh();
    }
}

void Model::loadNode(aiNode *node, const aiScene *scene) {
    // Iterating over meshes
    for(size_t i = 0; i < node->mNumMeshes; i++) {
        // Actual Mesh is stored in scene
        // Node contains the data - node->mMeshes[i]
        loadMesh(scene->mMeshes[node->mMeshes[i]], scene);
    }

    // Iterating over children of meshes
    for(size_t i = 0; i < node->mNumChildren; i++) {
        loadNode(node->mChildren[i], scene);
    }
}

void Model::loadMesh(aiMesh * mesh, const aiScene * scene) {
    std::vector<GLfloat> vertices;
    std::vector<unsigned int> indices;

    for(size_t i = 0; i < mesh->mNumVertices; i++) {
        // Recreating the array we made in main.cpp for Vertices, UVs and Normals
        vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });

        // Checking if mesh has texture
        if(mesh->mTextureCoords[0]) {
            vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
        }

        else{
            vertices.insert(vertices.end(), { 0.0f, 0.0f });
        }

        // Adding Normals
        // Adding the reversed values because of the shader code
        vertices.insert(vertices.end(), { -mesh->mNormals[i].x, -mesh->mNormals[i].y, -mesh->mNormals[i].z });
    }

    // Adding indices
    // Going through each face as it has 3 vertices (indices)
    for(size_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        for(size_t j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    Mesh* newMesh = new Mesh();
    newMesh->createMesh( &vertices[0], &indices[0], vertices.size(), indices.size() );
    meshList.push_back(newMesh);

    // Storing index of all materials
    meshToTex.push_back(mesh->mMaterialIndex);
}

void Model::loadMaterials(const aiScene * scene) {
    textureList.resize(scene->mNumMaterials);

    for(size_t i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* material = scene->mMaterials[i];

        textureList[i] = nullptr;

        // Checking for null maps
        if(material->GetTextureCount(aiTextureType_DIFFUSE)) {
            aiString path;

            // If there is a map
            if(material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
                int idx = std::string(path.data).rfind("\\");
                std::string fileName = std::string(path.data).substr(idx + 1);

                std::string texPath = std::string("D:/Programs/C++/Yumi/src/Base/Textures/") + fileName;

                textureList[i] = new Texture(texPath.c_str());

                if(!textureList[i]->loadTexture()) {
                    printf("Failed to load texture at: %s\n", texPath.c_str());

                    // SAFE_DELETE
                    delete textureList[i];
                    textureList[i] = nullptr;
                }
            }
        }

        // If there was no texture, plugin default texture
        if(!textureList[i]) {
            textureList[i] = new Texture("D:/Programs/C++/Yumi/src/Base/Textures/brickHi.png");
            textureList[i]->loadTextureA();
        }
    }
}

void Model::loadModel(const std::string filePath) {
    Assimp::Importer importer;

    // aiProcess_Triangulate - Triangulate quads or mesh
    // aiProcess_FlipUVs - Flip UVs along Y axis (Because of the way our lighting is setup)
    // aiProcess_GenSmoothNormals - We are not handling flat shading
    // aiProcess_JoinIdenticalVertices - If overlapping vertices, will combine them
    const aiScene *scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

    // If there is no model
    if(!scene) {
        printf("Failed to load model (%s) : %s\n", filePath.c_str(), importer.GetErrorString());
        return;
    }

    loadNode(scene->mRootNode, scene);

    loadMaterials(scene);
}

void Model::clearModel() {
    for(size_t i = 0; i < meshList.size(); i++) {
        if(meshList[i]) {
            delete meshList[i];
            meshList[i] = nullptr;
        }
    }

    for(size_t i = 0; i < textureList.size(); i++) {
        if(textureList[i]) {
            delete textureList[i];
            textureList[i] = nullptr;
        }
    }
}

Model::~Model() {
}
