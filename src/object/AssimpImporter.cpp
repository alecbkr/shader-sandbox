#include "AssimpImporter.hpp"

#include <vector>
#include <memory>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "core/logging/Logger.hpp"
#include "ModelCache.hpp"
#include "MaterialCache.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/InspectorEngine.hpp"
#include "application/Project.hpp"

#include <iostream> //TEMPADD

static const aiTextureType TexMap[13] {
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


AssimpImporter::AssimpImporter() {

}


bool AssimpImporter::initialize(Logger* _loggerPtr, ModelCache* _modelCachePtr, MaterialCache* _materialCachePtr, ShaderRegistry* _shaderRegPtr, InspectorEngine* _inspectorEngPtr, Project* projectData) {
    loggerPtr        = _loggerPtr;
    modelCachePtr    = _modelCachePtr;
    materialCachePtr = _materialCachePtr;
    shaderRegPtr     = _shaderRegPtr;
    inspectorEngPtr  = _inspectorEngPtr; 

    loadAssetCachesFromSave(projectData->modelData, projectData->materialData);
    return true;
}


bool AssimpImporter::loadAssetCachesFromSave(std::vector<ModelEntry>& modelEntries, std::vector<MaterialEntry>& materialEntries) {
    std::string feedback = "";

    // LOAD MATERIALS
    for (MaterialEntry& materialEntry : materialEntries) {
        unsigned int ID = materialEntry.ID;
        std::string name = materialEntry.name;
        MaterialType type = materialEntry.type;
        MaterialProperties properties = materialEntry.properties;
        std::vector<std::string>& texture_paths = materialEntry.texture_paths;

        bool loadResult = materialCachePtr->loadMaterialFromSave(ID, type, properties, texture_paths);
        if (loadResult == true) {
            materialCachePtr->getMaterial(ID)->setProgramName(materialEntry.programName);
            materialCachePtr->changeMaterialName(ID, name);
        }
        else {
            feedback += "Material failed to load: \"" + name + "\"\n";
        }
    }
    
    for (ModelEntry& modelEntry : modelEntries) {

        std::string name = modelEntry.name;
        unsigned int ID = modelEntry.ID;
        std::filesystem::path path = modelEntry.path;
        ModelType type = modelEntry.type;

        std::vector<unsigned int> meshMaterialIDs = modelEntry.meshMaterialIDs;
        std::vector<InstanceData> instanceData = modelEntry.instanceData;
        glm::vec3 position = modelEntry.position;
        glm::vec3 scale = modelEntry.scale;
        // glm::vec4 rotation = modelEntry.rotation;

        // LOAD MESHES
        bool reservationResult = modelCachePtr->reserveModelID(ID, path.string(), type);
        if (reservationResult == false) {
            feedback += "Model failed to load: \"" + name + "\". Reservation failure\n";
            continue;
        }

        if (type != ModelType::Imported) {
            modelCachePtr->addPresetMesh(ID, type);
        }
        else {
            Assimp::Importer import;
            const aiScene *scene = import.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                feedback += "Model failed to load: \"" + name + "\"" + "path not found " + path.string() + "\n";
                continue;
            }
            processNode(ID, scene->mRootNode, scene);
        }
        Model* model = modelCachePtr->getModel(ID);
        model->finalizeMeshes();

        unsigned int modelNumber = 0;
        std::string extraIdentification = "";
        while (modelCachePtr->changeModelName(ID, (name + extraIdentification)) == false) {
            extraIdentification = std::to_string(++modelNumber);
        }

        // LOAD BOUND MATERIALS PER MESH
        // model->loadMeshMaterialIDs(meshMaterialIDs);
        for (unsigned int idx = 0; idx < meshMaterialIDs.size(); idx++) {
            unsigned int materialID = meshMaterialIDs[idx];
            if (materialCachePtr->getMaterialIDMap().contains(materialID)) {
                model->setMeshMaterial(idx, materialID, materialCachePtr->getMaterial(materialID)->getValidity());
            }
            else if (materialID != std::numeric_limits<unsigned int>::max()){
                feedback += "model \"" + name + "\" has missing material";
            }
        }
        model->setPosition(position);
        model->setScale(scale);
        // model->setRotation(rotation); //nd
        model->setInstanceCount(instanceData.size());
        model->loadInstanceData(instanceData);

        bool success = modelCachePtr->updateRenderer(ID);

        // inspectorEngPtr->refreshUniforms();
    }
    if (!feedback.empty()) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "ASSIMPIMPORTER::loadAssetCachesFromSave()\n", feedback);
    }
    else {
        loggerPtr->addLog(LogLevel::INFO, "ASSIMPIMPORTER::loadAssetCachesFromSave()", "Asset caches loaded successfully");
    }

    return true;
}


unsigned int AssimpImporter::importModel(std::string path) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        loggerPtr->addLog(LogLevel::WARNING, "ASSIMP_IMPORTER::importModel()", "Model not found");
        return INVALID_MODEL_ID;
    }

    unsigned int modelID = modelCachePtr->createModelForImportSetup(path);
    std::string directory = path.substr(0, path.find_last_of('/'));
    
    // GRAB MATERIALS -- starts as 1 to avoid creating an unused default mat from assimp
    for (unsigned int i = 1; i < scene->mNumMaterials; i++) {
        processMaterial(scene->mMaterials[i], directory);
    }

    // PROCESS MESHES
    processNode(modelID, scene->mRootNode, scene);
    modelCachePtr->getModel(modelID)->finalizeMeshes();
    modelCachePtr->updateRenderer(modelID);
    return modelID;
}


void AssimpImporter::processNode(unsigned int modelID, aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        
        aiMesh *aimesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(modelID, aimesh);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(modelID, node->mChildren[i], scene);
    }
}


void AssimpImporter::processMesh(unsigned int modelID, aiMesh *aimesh) {
    MeshProperties meshflags{
        aimesh->HasPositions(),
        aimesh->HasNormals(),
        aimesh->HasTextureCoords(0)
    };

    // VERTICES
    std::vector<Vertex> vertices;
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

        // if (meshflags.hasColors) {
        //     glm::vec4 vector4;
        //     vector4.r = aimesh->mColors[0][i].r;
        //     vector4.g = aimesh->mColors[0][i].g;
        //     vector4.b = aimesh->mColors[0][i].b;
        //     vector4.a = aimesh->mColors[0][i].a;
        //     vertex.color = vector4;
        // }

        vertices.push_back(vertex);
    }

    // INDICES
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < aimesh->mNumFaces; i++) {
        aiFace face = aimesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    
    Model* importedModel = modelCachePtr->getModel(modelID);
    importedModel->addMeshByAssimp(vertices, indices, meshflags.hasPositions, meshflags.hasNormals, meshflags.hasUVs);
}


void AssimpImporter::processMaterial(aiMaterial *aimat, std::string directory) {

    MaterialProperties properties;
    aimat->Get(AI_MATKEY_OPACITY, properties.opacity);
    aimat->Get(AI_MATKEY_SHININESS, properties.shininess);
    aimat->Get(AI_MATKEY_ROUGHNESS_FACTOR, properties.roughness);
    aimat->Get(AI_MATKEY_METALLIC_FACTOR, properties.metalness);


    unsigned int materialID = materialCachePtr->createBlankMaterial();
    Material* newMaterial = materialCachePtr->getMaterial(materialID);
    newMaterial->setName("Imported_Mat");
    // newMaterial->setProperties(properties);
    getTextures(materialID, aimat, directory);
}


void AssimpImporter::getTextures(unsigned int materialID, aiMaterial *mat, std::string directory) {
    
    for (unsigned int type = 0; type < 13; type++) {

        aiTextureType aiType = TexMap[type];
        if (aiType == aiTextureType_NONE) continue;

        for (unsigned int idx = 0; idx < mat->GetTextureCount(aiType); idx++) {
            aiString aiTex;
            if (mat->GetTexture(aiType, idx, &aiTex) != AI_SUCCESS) {
                // Logger::addLog(LogLevel::ERROR, "ASSIMP_IMPORT", "Assimp failed to get texture");
                continue;
            }

            std::string filepath = directory + "/" + aiTex.C_Str();
            materialCachePtr->addTextureToMaterial(materialID, filepath, false);
        }
    }
}
