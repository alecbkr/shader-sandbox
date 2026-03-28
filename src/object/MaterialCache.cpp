#include "MaterialCache.hpp"

#include "core/logging/Logger.hpp"
#include "core/EventDispatcher.hpp"
#include "object/Material.hpp"
#include "texture/TextureCache.hpp"
#include <memory>

MaterialCache::MaterialCache() = default;

bool MaterialCache::initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, TextureCache* _textureCachePtr, bool previouslySaved) {
    loggerPtr = _loggerPtr;
    eventsPtr = _eventsPtr;
    textureCachePtr = _textureCachePtr;

    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Material Cache Initialization", "Material Cache was already initialized.");
        return false;
    }
    

    initialized = true;
    return true;
}

void MaterialCache::shutdown() {
    loggerPtr   = nullptr;
    initialized = false;
}

unsigned int MaterialCache::createBlankMaterial() {
    if (materialIDMap.contains(nextMaterialID)) {
        // This should never happen. material sets it's own ID using MateralCache::getNextMaterialID
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MaterialCache::createMaterial",  "overlapping material IDs! this should never happen!");
    }
    unsigned int newMaterialID = nextMaterialID;
    materialIDMap.emplace(newMaterialID, std::make_unique<Material>(nextMaterialID, MaterialType::Opaque));
    nextMaterialID++;
    return newMaterialID;
}


unsigned int MaterialCache::createMaterial(MaterialType type, MaterialProperties properties, std::vector<unsigned int> textureIDs) {
    if (validateNextID() == false) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MaterialCache::createMaterial",  "could not find a free next ID");
    }

    unsigned int newMaterialID = nextMaterialID;
    materialIDMap.emplace (newMaterialID, std::make_unique<Material>(nextMaterialID, type, properties, textureIDs));
    nextMaterialID++;

    return newMaterialID;
}


void MaterialCache::addTextureToMaterial(unsigned int materialID, std::string texture_path) {
    Material* foundMaterial = getMaterial(materialID);
    if (foundMaterial == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "addTextureToMaterial", "materialID out of bounds: ", std::to_string(materialID));
        return;
    }
    unsigned int textureID = textureCachePtr->createTexture2D(texture_path);
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


bool MaterialCache::loadMaterialFromSave(unsigned int materialID, MaterialType type, MaterialProperties properties, std::vector<std::string> texture_paths) {
    if (materialIDMap.contains(materialID)) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MATERIALCACHE | loadMaterialsFromSave", "reservation failed. material already exists with ID " + std::to_string(materialID));
        return false;
    }
    materialIDMap.emplace(materialID, std::make_unique<Material>(materialID, type));
    for(std::string texture_path : texture_paths) {
        addTextureToMaterial(materialID, texture_path);
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