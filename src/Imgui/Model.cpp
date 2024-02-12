#include "Model.h"

Model::Model() {
}

void Model::renderModel() {
    // Iterating over the mesh to render
    for(size_t i = 0; i < meshList.size(); i++) {
        unsigned int materialIndex = meshToTex[i];

        // Debugging
        // printf("Material Index : %i, Texture List Size : %i", materialIndex, textureList.size());

        // Iterate over all textures in textureList
        for (size_t index = 0; index < textureList.size(); index++) {
            // Checking if the texure exists
            if (textureList[index]) {
                // printf("Render Model : %i\n", index);
                textureList[index]->useTexture(index);
            }
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

// Function to load the maps, since the functionality for loading each map is similar
void Model::loadMap(aiMaterial* material, aiTextureType textureType, int textureIndex) {
    aiString path;

    // If there is a diffuse map
    if(material->GetTexture(textureType, 0, &path) == AI_SUCCESS) {
        int idx = std::string(path.data).rfind("\\");
        std::string fileName = std::string(path.data).substr(idx + 1);

        std::string texPath = std::string("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/") + fileName;

        // Debugging
        // printf("Loading Texture from: %s\n", texPath.c_str());

        textureList[textureIndex] = new Texture(texPath.c_str());

        // Debugging
        // printf("Loading Texture: %s\n", fileName.c_str());

        if(!textureList[textureIndex]->loadTexture()) {
            printf("Failed to load texture at: %s\n", texPath.c_str());

            // SAFE_DELETE
            delete textureList[textureIndex];
            textureList[textureIndex] = nullptr;

            // Loading an empty Normal Map instead of the default white texture
            if(textureType == aiTextureType_HEIGHT) {
                // Debugging
                // printf("Adding default Normal Map\n");

                textureList[textureIndex] = new Texture("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/emptyNormal.png");
                textureList[textureIndex]->loadTexture();
            }
        }
    }
}

void Model::loadMaterials(const aiScene * scene) {
    // Initializing the vector
    textureList.resize(scene->mNumMaterials);

    // Debugging
    // printf("Number of Materials : %i\n", scene->mNumMaterials);

    // Checking if already loaded
    bool diffuse = true, specular = true, normal = true, ambientOcclusion = true;

    /*
    The code is not using AI_SCENE_FLAGS_INCOMPLETE, hence it will always have 1 material by default
    To bypass this, iterating over the materials by checking if numMaterials > 1 and using those as texture.
    */
    if(scene->mNumMaterials > 1) {
        // TODO : Remove the magic number 1
        aiMaterial* material = scene->mMaterials[1];

        // Counting the various textures in the material
        int textureCount = material->GetTextureCount(aiTextureType_DIFFUSE) +
                           material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) +
                           material->GetTextureCount(aiTextureType_SPECULAR) +
                           material->GetTextureCount(aiTextureType_HEIGHT);

        // Debugging
        // printf("Texture Count : %i\n", textureCount);

        // Resizing list to fit all the textures
        textureList.resize(textureCount);

        // Iterating over the various textures
        for(size_t i = 0; i < textureCount; i++) {
            textureList[i] = nullptr;

            // Debugging
            // printf("Texture Index : %zi\n", i);

            // Checking for null maps
            // DIFFUSE MAP - Texture Unit 0
            if(material->GetTextureCount(aiTextureType_DIFFUSE) && diffuse) {
                loadMap(material, aiTextureType_DIFFUSE, i);
                diffuse = false;
            }

            // SPECULAR MAP - Texture Unit 1
            else if(material->GetTextureCount(aiTextureType_SPECULAR) && specular) {
                loadMap(material, aiTextureType_SPECULAR, i);
                specular = false;
            }

            // NORMAL MAP - Texture Unit 2
            // aiTextureType_NORMAL doesn't load normal maps, aiTextureType_HEIGHT does - Wavefront OBJ format
            else if(material->GetTextureCount(aiTextureType_HEIGHT) && normal) {
                loadMap(material, aiTextureType_HEIGHT, i);
                normal = false;
            }

            // AMBIENT OCCLUSION - Texture Unit 3
            else if(material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) && ambientOcclusion) {
                loadMap(material, aiTextureType_AMBIENT_OCCLUSION, i);
                ambientOcclusion = false;
            }

            // If there was no texture, plugin default texture
            if(!textureList[i]) {
                // Debugging
                // printf("Adding default White Texture\n");

                textureList[i] = new Texture("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/white.jpg");
                textureList[i]->loadTexture();
            }
        }
    }

    // Default material/No material
    else if(scene->mNumMaterials == 1) {
        aiMaterial* material = scene->mMaterials[0];

        textureList[0] = nullptr;

        // Debugging
        // printf("Missing materials!\nAdding default texture\n");

        textureList[0] = new Texture("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/white.jpg");
        textureList[0]->loadTexture();
    }

    else {
        printf("Error loading materials!\n");
    }

    // Debugging
    // printf("Texture List Size : %i\n", textureList.size());
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
