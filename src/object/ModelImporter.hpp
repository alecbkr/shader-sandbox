#pragma once

#include "TextureType.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <memory>
#include <string>
#include "MeshAssimp.hpp"
#include "Vertex.hpp"
#include "ImportedModel.hpp"

class Logger;

struct ImportContext {
    std::string directory;
};


static const aiTextureType TexMap[TEX_MAXTYPE] {
    aiTextureType_NONE,              // TEX_UNDEFINED
    aiTextureType_DIFFUSE,           // TEX_DIFFUSE
    aiTextureType_SPECULAR,          // TEX_SPECULAR
    aiTextureType_NORMALS,           // TEX_NORMAL
    aiTextureType_HEIGHT,            // TEX_HEIGHT
    aiTextureType_EMISSIVE,          // TEX_EMISSIVE
    aiTextureType_OPACITY,           // TEX_OPACITY
    aiTextureType_AMBIENT,           // TEX_AMBIENT
    aiTextureType_BASE_COLOR,        // TEX_BASECOLOR
    aiTextureType_METALNESS,         // TEX_METALNESS
    aiTextureType_DIFFUSE_ROUGHNESS, // TEX_ROUGHNESS
    aiTextureType_AMBIENT_OCCLUSION, // TEX_AO
    aiTextureType_EMISSION_COLOR     // TEX_EMISSION
};

class ModelImporter {
public:
    ModelImporter();
    bool initialize(Logger* _loggerPtr);
    void shutdown();
    bool importModel(std::string path, ImportedModel& model);
    void processNode(aiNode *node, const aiScene *scene, ImportedModel& model, ImportContext& ctx);
    void processMesh(aiMesh *aimesh, const aiScene *scene, ImportedModel& model, ImportContext& ctx);
    void loadTextures(aiMaterial *mat, std::vector<std::shared_ptr<Texture>>& meshTextures, ImportedModel& model, ImportContext& ctx);
private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
};