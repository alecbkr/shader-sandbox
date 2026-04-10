#include "ModelCache.hpp"

#include <memory>
#include <algorithm>
#include <string>

#include "core/InspectorEngine.hpp"
#include "core/UniformRegistry.hpp"
#include "core/EventDispatcher.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/logging/Logger.hpp"
#include "object/MaterialCache.hpp"
#include "presets/PresetAssets.hpp"


ModelCache::ModelCache() {
    initialized = false;
    loggerPtr = nullptr;
    eventsPtr = nullptr;
    presetsPtr = nullptr;
}

bool ModelCache::initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr,  PresetAssets* _presetsPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Model Cache Initialization", "Model Cache was already initialized.");
        return false;
    }
    loggerPtr       = _loggerPtr;
    eventsPtr       = _eventsPtr;
    presetsPtr      = _presetsPtr;

    eventsPtr->Subscribe(EventType::ReloadShader, [this](const EventPayload& payload) -> bool {
        
        if (const auto* data = std::get_if<ReloadShaderPayload>(&payload)) {
            
            std::string programName = data->programName;
            for (auto& [ID, model] : modelIDMap) {
                if (model->getProgramID() == programName) {
                    model->setProgramID(programName);
                }
            }
            // reorderByProgram();
            return true;
        }
        return false;
    });

    eventsPtr->Subscribe(EventType::MaterialValidated, [this](const EventPayload& payload) -> bool {
        if (const auto* data = std::get_if<MaterialValidatedPayload>(&payload)) {
            for (const auto& [ID, model] : modelIDMap) {
                if (model->getInvalidMaterialIDs().erase(data->materialID)) {
                    if (model->getInvalidMaterialIDs().empty()) {
                    model->getModelStatus().material = ModelState::Ready;
                    updateRenderer(ID);
                    }
                }
            }
            return true;
        }
        return false;
    });

    eventsPtr->Subscribe(EventType::MaterialsInvalidated, [this](const EventPayload& payload) -> bool {
        if (const auto* data = std::get_if<MaterialsInvalidatedPayload>(&payload)) {
            for (const auto& [ID, model] : modelIDMap) {
                for (unsigned int materialID : data->invalidMaterialIDs) {
                    if (model->getAllMaterialReferences().contains(materialID)) {
                        model->getInvalidMaterialIDs().insert(materialID);
                    }
                }
                if (!model->getInvalidMaterialIDs().empty()) {
                    model->getModelStatus().material = ModelState::Invalid;
                    updateRenderer(ID);
                }
            }
            return true;
        }
        return false;
    });

    initialized = true;
    return true;
}

void ModelCache::shutdown() {
    loggerPtr       = nullptr;
    eventsPtr       = nullptr;
    initialized = false;
}


unsigned int ModelCache::createPreset(ModelType type) {
    if (validateNextID() == false) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE::createPreset()", "failed to validate next model ID on model creation");
        return INVALID_MODEL_ID;
    }

    if (type == ModelType::Imported) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE::createPreset()", "invalid type call, imported model types are reserved for the importer");
        return INVALID_MODEL_ID;
    }

    unsigned int presetModelID = nextModelID;
    modelIDMap.emplace(presetModelID, std::make_unique<Model>(presetModelID, "preset", type));
    addPresetMesh(presetModelID, type);
    getModel(presetModelID)->finalizeMeshes();

    unsigned int modelNumber = nextModelID;
    while (changeModelName(presetModelID, "Model" + std::to_string(modelNumber)) == false) {
        modelNumber++;
    }

    updateRenderer(presetModelID);
    nextModelID++;
    return presetModelID;
}


bool ModelCache::changeModelName(unsigned int modelID, std::string name) {
    Model* foundModel = getModel(modelID);
    if (foundModel == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "MODELCACHE::changeModelName", "model not found with ID " + std::to_string(modelID));
        return false;
    }
    if (usedModelNames.contains(name)) {
        loggerPtr->addLog(LogLevel::WARNING, "MODELCACHE::changeModelName", "new name must be unique");
        return false;
    }

    usedModelNames.erase(foundModel->getName());
    foundModel->setName(name);
    usedModelNames.insert(name);
    return true;
}


void ModelCache::deleteModel(unsigned int modelID) {
    Model* model = getModel(modelID);
    if (model == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "MODELCACHE::deleteModel()", "Model not found with ID " + std::to_string(modelID));
        return;
    }

    if (modelID == skyboxModelID) {
        skyboxModelID = INVALID_MODEL_ID;
    }
    
    modelIDMap.erase(modelID);
    eventsPtr->TriggerEvent(Event { EventType::DeleteFromRenderer, false, DeleteFromRendererPayload{modelID} });
}


void ModelCache::toggleAsSkybox(unsigned int modelID) {
    Model* model = getModel(modelID);
    if (model == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "MODELCACHE::setAsSkyBox()", "Model not found with ID " + std::to_string(modelID));
        return;
    }
    if (model->type != ModelType::CubePreset) {
        loggerPtr->addLog(LogLevel::WARNING, "MODELCACHE::setAsSkyBox()", "Model given is not of the type CubePreset");
        return;
    }

    modelID == skyboxModelID ? skyboxModelID = INVALID_MODEL_ID : skyboxModelID = modelID;
    updateRenderer(modelID);
}


void ModelCache::changeMeshMaterial(unsigned int modelID, unsigned int meshIdx, unsigned int materialID, bool isMatValid) {
    Model* foundModel = getModel(modelID);
    if (foundModel == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE::changeMeshMaterial()", "model not found from ID " + std::to_string(modelID));
        return;
    }

    unsigned int numOfMeshes = foundModel->getNumberOfMeshes();
    if (meshIdx < 0 || numOfMeshes < meshIdx) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE::changeMeshMaterial()", "mesh idx out of bounds " + std::to_string(meshIdx));
        return;
    }

    foundModel->setMeshMaterial(meshIdx, materialID, isMatValid);
    updateRenderer(modelID);
}


void ModelCache::changeModelMaterial(unsigned int modelID, unsigned int materialID, bool isMatValid) {
    Model* foundModel = getModel(modelID);
    if (foundModel == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE::changeModelMaterial()", "model not found from ID " + std::to_string(modelID));
        return;
    }
    foundModel->setModelMaterial(materialID, isMatValid);
    
    updateRenderer(modelID);
}


std::vector<Model*> ModelCache::getAllModels() const {
    std::vector<Model*> models;
    models.reserve(modelIDMap.size());
    for (auto& [id, modelPtr] : modelIDMap) {
        models.push_back(modelPtr.get());
    }
    return models;
}


Model* ModelCache::getModel(unsigned int modelID) {
    auto it = modelIDMap.find(modelID);
    if (it == modelIDMap.end()) {
        return nullptr;
    }
    return it->second.get();
}


unsigned int ModelCache::getSkyboxModelID() const {
    return skyboxModelID;
}


int ModelCache::getNumberOfModels() {
    return modelIDMap.size();
}


void ModelCache::addPresetMesh(unsigned int modelID, ModelType type) {
    MeshPreset preset;
    switch(type) {
        case ModelType::PlanePreset: preset = MeshPreset::PLANE; break;
        case ModelType::PyramidPreset: preset = MeshPreset::PYRAMID; break;
        case ModelType::CubePreset: preset = MeshPreset::CUBE; break;
        default: preset = MeshPreset::CUBE;
    }

    MeshData presetData = presetsPtr->getPresetMesh(preset);
    Model* presetModel = getModel(modelID);
    presetModel->addMeshByData(presetData.verts, presetData.indices, true, false, true);
}


unsigned int ModelCache::createModelForImportSetup(std::string model_path) {
    if (validateNextID() == false) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE::createEmptyModel()", "failed to validate next model ID on model creation");
        return INVALID_MODEL_ID;
    }
    unsigned int emptyModelID = nextModelID;
    modelIDMap.emplace(nextModelID, std::make_unique<Model>(emptyModelID, model_path, ModelType::Imported));
    
    unsigned int modelNumber = nextModelID;
    while (changeModelName(emptyModelID, "Model" + std::to_string(modelNumber)) == false) {
        modelNumber++;
    }

    nextModelID++;
    return emptyModelID;
}


bool ModelCache::reserveModelID(unsigned int modelID, std::string model_path, ModelType type) {
    if (modelIDMap.contains(modelID)) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE::reserveModelID()", "reservation failed. ModelCache already contains ID " + std::to_string(modelID));
        return false;
    }
    modelIDMap.emplace(modelID, std::make_unique<Model>(modelID, model_path, type));
    return true;
}


bool ModelCache::updateRenderer(unsigned int modelID) {
    ModelStatus& modelStatus = getModel(modelID)->getModelStatus();
    if (modelStatus.uploadedToRenderer == true) {
        eventsPtr->TriggerEvent(Event { EventType::DeleteFromRenderer, false, DeleteFromRendererPayload{modelID}});
    } 

    if (modelStatus.meshes != ModelState::Ready || modelStatus.material != ModelState::Ready)  {
        if (modelStatus.meshes == ModelState::Error) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE::updateRenderer()", "mesh is in error state for model " + std::to_string(modelID));
        }
        if (modelStatus.material == ModelState::Error) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE::updateRenderer()", "material(s) in error state for model " + std::to_string(modelID));
        }
        return false;
    }

    bool isSkybox;
    modelID == skyboxModelID ? isSkybox = true : isSkybox = false;

    eventsPtr->TriggerEvent(Event { EventType::UploadToRenderer, false, UploadToRendererPayload{modelID, isSkybox} });
    return true;
}


bool ModelCache::validateNextID() {
    unsigned int numOfChecks = 0;
    while (modelIDMap.contains(nextModelID) == true) {
        if (numOfChecks > 1000) return false;
        
        nextModelID++;
        numOfChecks++;
    }
    return true;
}