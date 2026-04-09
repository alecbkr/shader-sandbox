#pragma once

#include <string>
#include "application/Project.hpp"

class aiScene;
class aiNode;
class aiMesh;
class aiMaterial;

class Logger;
class ModelCache;
class MaterialCache;
class ShaderRegistry;
class InspectorEngine;
struct Project;

struct ImportContext;

class AssimpImporter {
public:
    AssimpImporter();
    bool initialize(Logger* _loggerPtr, ModelCache* _modelCachePtr, MaterialCache* _materialCachePtr, ShaderRegistry* _shaderRegPtr, InspectorEngine* _inspectorEngPtr, Project* _projectData);
    bool loadAssetCachesFromSave(std::vector<ModelEntry>& modelEntries, std::vector<MaterialEntry>& materialEntries);
    unsigned int importModel(std::string model_path);

    

private:
    void processNode(unsigned int modelID, aiNode* node, const aiScene* scene);
    void processMesh(unsigned int modelID, aiMesh* aimesh);
    void processMaterial(aiMaterial* aimat, std::string directory);
    void getTextures(unsigned int materialID, aiMaterial* aimat, std::string directory);

    //SYSTEM POINTERS
    Logger* loggerPtr                = nullptr;
    ModelCache* modelCachePtr        = nullptr;
    MaterialCache* materialCachePtr  = nullptr;
    ShaderRegistry* shaderRegPtr     = nullptr;
    InspectorEngine* inspectorEngPtr = nullptr;

};