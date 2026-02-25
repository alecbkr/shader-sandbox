#include "object/ModelImporter.hpp"

#include "core/logging/Logger.hpp"

ModelImporter::ModelImporter() {
    initialized = false;
    loggerPtr = nullptr;
}

bool ModelImporter::initialize(Logger* _loggerPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Model Importer Initialization", "Model Importer was already initialized.");
        return false;
    }

    loggerPtr = _loggerPtr;
    initialized = true;
    return true;
}

void ModelImporter::shutdown() {
    if (!initialized) return;
    loggerPtr = nullptr;
    initialized = false;
}

bool ModelImporter::importModel(std::string path, ImportedModel& model) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        loggerPtr->addLog(LogLevel::WARNING, "MODEL_IMPORTER", "Model not found");
        return false;
    }

    ImportContext ctx;

    ctx.directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene, model, ctx);

    return true;
}

void ModelImporter::processNode(aiNode *node, const aiScene *scene, ImportedModel& model, ImportContext &ctx) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {

        aiMesh *aimesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(aimesh, scene, model, ctx);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, model, ctx);
    }
}

void ModelImporter::processMesh(aiMesh *aimesh, const aiScene *scene, ImportedModel& model, ImportContext &ctx) {


    MeshFlags meshflags{
        aimesh->HasPositions(),
        aimesh->HasNormals(),
        aimesh->HasTextureCoords(0),
        aimesh->HasVertexColors(0),
        aimesh->mMaterialIndex >= 0 ? true : false // textures
    };
    

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::shared_ptr<Texture>> textures;

    // VERTICES
    for (unsigned int i = 0; i < aimesh->mNumVertices; i++) {
        Vertex vertex;
        
        if (meshflags.hasPositions) {
            glm::vec3 vector3;
            vector3.x = aimesh->mVertices[i].x;
            vector3.y = aimesh->mVertices[i].y;
            vector3.z = aimesh->mVertices[i].z;
            vertex.position = vector3;
        }
        
        if (meshflags.hasNormals) {
            glm::vec3 vector3;
            vector3.x = aimesh->mNormals[i].x;
            vector3.y = aimesh->mNormals[i].y;
            vector3.z = aimesh->mNormals[i].z;
            vertex.normal = vector3;
        }

        if (meshflags.hasUVs) {
            glm::vec2 vector2;
            vector2.x = aimesh->mTextureCoords[0][i].x;
            vector2.y = aimesh->mTextureCoords[0][i].y;
            vertex.uv = vector2;
        }

        if (meshflags.hasColors) {
            glm::vec4 vector4;
            vector4.r = aimesh->mColors[0][i].r;
            vector4.g = aimesh->mColors[0][i].g;
            vector4.b = aimesh->mColors[0][i].b;
            vector4.a = aimesh->mColors[0][i].a;
        }

        vertices.push_back(vertex);
    }

    // INDICES
    for (unsigned int i = 0; i < aimesh->mNumFaces; i++) {
        aiFace face = aimesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    
    // TEXTURES
    if (meshflags.hasTextures) {
        aiMaterial *material = scene->mMaterials[aimesh->mMaterialIndex];
        loadTextures(material, textures, model, ctx);

        if (meshflags.hasUVs == false) {
            loggerPtr->addLog(LogLevel::WARNING, "MODEL IMPORT", "Mesh has textures but no texture coordinates");
        }
    }
    model.getMeshes().push_back(MeshA(vertices, indices, textures, meshflags));
}

void ModelImporter::loadTextures(aiMaterial *mat, std::vector<std::shared_ptr<Texture>>& textures, ImportedModel& model, ImportContext &ctx) {
    
    for (unsigned int type = 0; type < TEX_MAXTYPE; type++) {

        aiTextureType aiType = TexMap[type];
        if (aiType == aiTextureType_NONE) continue;

        for (unsigned int idx = 0; idx < mat->GetTextureCount(aiType); idx++) {
            aiString aiTex;
            if (mat->GetTexture(aiType, idx, &aiTex) != AI_SUCCESS) {
                loggerPtr->addLog(LogLevel::LOG_ERROR, "MODEL_IMPORT", "Assimp failed to get texture");
                continue;
            }

            std::string filepath = ctx.directory + "/" + aiTex.C_Str();
            if (model.getTextures().contains(filepath) == false) {
                loggerPtr->addLog(LogLevel::INFO, "loadTextures", "building texture: " + filepath);
                model.getTextures().try_emplace(filepath, std::make_shared<Texture>(filepath.c_str(), static_cast<TextureType>(type), loggerPtr));
            }
            
            
            textures.push_back(model.getTextures().at(filepath));
        }
    }
}