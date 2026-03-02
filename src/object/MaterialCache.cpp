#include "MaterialCache.hpp"
#include "core/logging/LogSink.hpp"
#include "core/logging/Logger.hpp"
#include "object/Material.hpp"
#include <memory>

MaterialCache::MaterialCache() = default;

bool MaterialCache::initialize(Logger* _loggerPtr) {
    loggerPtr       = _loggerPtr;
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Material Cache Initialization", "Material Cache was already initialized.");
        return false;
    }
    initialized = true;
    return true;
}

void MaterialCache::shutdown() {
    loggerPtr       = nullptr;
    initialized = false;
}

void MaterialCache::createMaterial(std::unique_ptr<Material> material) {
    if (materials.contains(material->ID)) {
        // This should never happen. material sets it's own ID using MateralCache::getNextMaterialID
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MaterialCache::createMaterial",  "overlapping material IDs! this should never happen!");
    }
    materials[material->ID] = std::move(material);
}

Material* MaterialCache::getMaterial(unsigned int materialID) {
    if (!materials.contains(materialID)) {
        return nullptr;
    }
    return materials[materialID].get();
}

bool MaterialCache::contains(unsigned int materialID) {
    return materials.contains(materialID);
};

unsigned int MaterialCache::getNextMaterialID() {
    unsigned int retVal = nextMaterialID;
    nextMaterialID++;
    return retVal;
}
