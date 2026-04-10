#include "MaterialCache.hpp"

#include "core/logging/Logger.hpp"
#include "core/EventDispatcher.hpp"
#include "object/Material.hpp"
#include "texture/TextureCache.hpp"
#include "object/ModelCache.hpp"
#include "object/Renderer.hpp"
#include "core/UniformRegistry.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/InspectorEngine.hpp"
#include <memory>
#include <limits>

#include <iostream> //TEMPADD

MaterialCache::MaterialCache() = default;

bool MaterialCache::initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, TextureCache* _textureCachePtr, ModelCache* _modelCachePtr, UniformRegistry* _uniformRegPtr, ShaderRegistry* _shaderRegPtr, bool previouslySaved) {
    loggerPtr = _loggerPtr;
    eventsPtr = _eventsPtr;
    textureCachePtr = _textureCachePtr;
    modelCachePtr = _modelCachePtr;
    uniformRegPtr = _uniformRegPtr;
    shaderRegPtr = _shaderRegPtr;

    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Material Cache Initialization", "Material Cache was already initialized.");
        return false;
    }

    eventsPtr->Subscribe(EventType::ProgramDeleted, [this](const EventPayload& payload) -> bool {
        
        if (const auto* data = std::get_if<ProgramDeletedPayload>(&payload)) {
            
            std::vector<unsigned int> invalidMaterialIDs;
            unsigned int programID = data->programID;
            for (auto& [ID, material] : materialIDMap) {
                if (material->getProgramID() == programID) {
                    material->setProgramID(std::numeric_limits<unsigned int>::max());
                    invalidMaterialIDs.push_back(ID);
                }
            }
            eventsPtr->TriggerEvent(Event{ EventType::MaterialsInvalidated, false, MaterialsInvalidatedPayload{ invalidMaterialIDs }});
            return true;
        }
        return false;
    });
    
    initialized = true;
    return true;
}

void MaterialCache::initializeAfterRenderer(Renderer* _rendererPtr, InspectorEngine* _inspectorEngPtr) {
    rendererPtr = _rendererPtr;
    inspectorEngPtr = _inspectorEngPtr;
}

void MaterialCache::shutdown() {
    loggerPtr   = nullptr;
    initialized = false;
}

unsigned int MaterialCache::createBlankMaterial() {
    if (validateNextID() == false) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MaterialCache::createMaterial",  "could not find a free next ID");
        return UINT_MAX;
    }

    unsigned int newMaterialID = nextMaterialID;
    materialIDMap.emplace(newMaterialID, std::make_unique<Material>(nextMaterialID, MaterialType::Opaque));

    unsigned int materialNumber = nextMaterialID;
    while (changeMaterialName(newMaterialID, "Material" + std::to_string(materialNumber)) == false) {
        materialNumber++;
    }
    nextMaterialID++;
    return newMaterialID;
}


unsigned int MaterialCache::createMaterial(MaterialType type, MaterialProperties properties, std::vector<unsigned int> textureIDs) {
    if (validateNextID() == false) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MaterialCache::createMaterial",  "could not find a free next ID");
        return UINT_MAX;
    }

    unsigned int newMaterialID = nextMaterialID;
    materialIDMap.emplace (newMaterialID, std::make_unique<Material>(nextMaterialID, type, properties, textureIDs));

    unsigned int materialNumber = nextMaterialID;
    while (changeMaterialName(newMaterialID, "Material" + std::to_string(materialNumber)) == false) {
        materialNumber++;
    }
    nextMaterialID++;
    return newMaterialID;
}

void MaterialCache::deleteMaterial(unsigned int materialID) {
    for (auto& model : modelCachePtr->getAllModels()) {
        for (auto& meshInstance : model->getMeshInstances()) {
            if (meshInstance.materialID == materialID) {
                model->setMeshMaterial(meshInstance.meshIdx, std::numeric_limits<unsigned int>::max(), true);
                rendererPtr->setMeshMaterial(model->getID(), meshInstance.meshIdx, std::numeric_limits<unsigned int>::max());
            }
        }
    }

    uniformRegPtr->eraseMaterial(materialID);
    materialIDMap.erase(materialID);
}


void MaterialCache::addTextureToMaterial(unsigned int materialID, std::string texture_path, bool isCubemap) {
    Material* foundMaterial = getMaterial(materialID);
    if (foundMaterial == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "addTextureToMaterial", "materialID out of bounds: ", std::to_string(materialID));
        return;
    }


    unsigned int textureID;
    switch (isCubemap) {
        case true:  textureID = textureCachePtr->createCubeMap(texture_path);   break;
        case false: textureID = textureCachePtr->createTexture2D(texture_path); break;
    }
    foundMaterial->addTexture(textureID);
}


void MaterialCache::removeTextureFromMaterial(unsigned int materialID, unsigned int textureID) {
    Material* foundMaterial = getMaterial(materialID);
    if (foundMaterial == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "removeTextureFromMaterial", "materialID out of bounds: ", std::to_string(materialID));
        return;
    }
    auto& textureIDs = foundMaterial->getMaterialTextureIDs();
    for (auto iter = textureIDs.begin(); iter != textureIDs.end(); ) {
        if (*iter == textureID) {
            iter = textureIDs.erase(iter);
        }
        else {
            iter++;
        }
    }
}


bool MaterialCache::changeMaterialName(unsigned int materialID, std::string name) {
    Material* foundMaterial = getMaterial(materialID);
    if (foundMaterial == nullptr) {
        return false;
    }
    if (usedMaterialNames.contains(name)) {
        return false;
    }

    usedMaterialNames.erase(foundMaterial->getName());
    foundMaterial->setName(name);
    usedMaterialNames.insert(name);
    return true;
}


void MaterialCache::changeMaterialType(unsigned int materialID, MaterialType type) {
    Material* foundMaterial = getMaterial(materialID);
    if (foundMaterial == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MATERIALCACHE | changeMaterialType", "material not found with ID" + std::to_string(materialID));
        return;
    }
    
    if (foundMaterial->getMaterialType() == type) return;

    foundMaterial->setMaterialType(type);
    eventsPtr->TriggerEvent(Event{ EventType::MaterialTypeChange, false, MaterialTypeChangePayload{materialID, type} });
}


void MaterialCache::changeMaterialProgram(unsigned int materialID, unsigned int programID) {
    Material* foundMaterial = getMaterial(materialID);
    if (foundMaterial == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MATERIALCACHE | changeMaterialProgram",
            "material not found with ID " + std::to_string(materialID));
        return;
    }

    if (foundMaterial->getProgramID() == programID) {
        return;
    }

    ShaderProgram* program = shaderRegPtr->getProgram(programID);
    if (program == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MATERIALCACHE | changeMaterialProgram",
            "program not found with ID: " + std::to_string(programID));
        return;
    }

    unsigned int oldProgramID = foundMaterial->getProgramID();
    foundMaterial->setProgramID(programID);

    if (oldProgramID == std::numeric_limits<unsigned int>::max()) {
        eventsPtr->TriggerEvent(Event{ EventType::MaterialValidated, false, MaterialValidatedPayload { materialID}});
    }

    // Clear old uniforms for this material, then let refreshUniforms rebuild them
    uniformRegPtr->eraseMaterial(materialID);

    inspectorEngPtr->refreshUniforms();
}


bool MaterialCache::loadMaterialFromSave(unsigned int materialID, MaterialType type, MaterialProperties properties, std::vector<std::string> texture_paths) {
    if (materialIDMap.contains(materialID)) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MATERIALCACHE | loadMaterialsFromSave", "reservation failed. material already exists with ID " + std::to_string(materialID));
        return false;
    }
    materialIDMap.emplace(materialID, std::make_unique<Material>(materialID, type));
    for(std::string texture_path : texture_paths) {
        addTextureToMaterial(materialID, texture_path, false);
    }
    getMaterial(materialID)->setProperties(properties);
    return true;
}


Material* MaterialCache::getMaterial(unsigned int materialID) {
    if (!materialIDMap.contains(materialID)) {
        return nullptr;
    }
    return materialIDMap.at(materialID).get();
}

bool MaterialCache::contains(unsigned int materialID) {
    return materialIDMap.contains(materialID);
};

std::vector<unsigned int> MaterialCache::getAllMaterialIDs() {
    std::vector<unsigned int> matIds;
    matIds.reserve(getSize());
    for (auto& [id, mat] : materialIDMap) {
        matIds.push_back(id);
    }

    return matIds;
}


std::vector<Material*> MaterialCache::getAllMaterials() {
    std::vector<Material*> materials;
    materials.reserve(getSize());
    for (auto& [id, material] : materialIDMap) {
        materials.push_back(material.get());
    }
    return materials;
}


std::vector<std::string> MaterialCache::getAllTexturePathsForMaterial(unsigned int materialID) {
    Material* material = getMaterial(materialID);
    if (material == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MATERIALCACHE | getAllTExturePathsForMaterial", "material not found with ID " + std::to_string(materialID));
    }
    std::vector<std::string> texturePaths;
    for (unsigned int textureID : material->getMaterialTextureIDs()) {
        texturePaths.push_back(textureCachePtr->getTexturePath(textureID));
    }
    return texturePaths;
}


int MaterialCache::getSize() {
    return materialIDMap.size();
}

unsigned int MaterialCache::getNextMaterialID() {
    unsigned int retVal = nextMaterialID;
    nextMaterialID++;
    return retVal;
}


bool MaterialCache::validateNextID() {
    unsigned int numOfChecks = 0;
    while (materialIDMap.contains(nextMaterialID) == true) {
        if (numOfChecks > 1000) return false;
        
        nextMaterialID++;
        numOfChecks++;
    }
    return true;
}

void MaterialCache::updateMatIDs() {
    for (auto& [ID, mat] : materialIDMap) {
        ShaderProgram* prog = shaderRegPtr->getProgram(mat->getProgramName());
        if (prog) {
            mat->setProgramID(prog->ID);
        }
    }
}