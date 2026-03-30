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
    trySendingToRenderer(presetModelID);
    nextModelID++;
    return presetModelID;
}


//DO NOT CALL THIS OUTSIDE MODELIMPORTER
unsigned int ModelCache::createModelForImportSetup(std::string model_path) {
    if (validateNextID() == false) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE::createEmptyModel()", "failed to validate next model ID on model creation");
        return INVALID_MODEL_ID;
    }
    unsigned int emptyModelID = nextModelID;
    modelIDMap.emplace(nextModelID, std::make_unique<Model>(emptyModelID, model_path, ModelType::Imported));
    
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


void ModelCache::trySendingToRenderer(unsigned int modelID) {
    ModelStatus& status = getModel(modelID)->getModelStatus();
    if (status.meshes != ModelState::Ready || status.material != ModelState::Ready)  {
        // if (status.meshes != ModelState::Ready) {
        //     loggerPtr->addLog(LogLevel::WARNING, "MODELCACHE::trySendingToRenderer()", "mesh is not set or has an error");
        // }
        // if (status.material != ModelState::Ready) {
        //     loggerPtr->addLog(LogLevel::WARNING, "MODELCACHE::trySendingToRenderer()", "material is not set or has an error");
        // }
        return;
    }

    bool isSkybox;
    modelID == skyboxModelID ? isSkybox = true : isSkybox = false;

    eventsPtr->TriggerEvent(Event { EventType::CreateModel, false, ModelCreationPayload{modelID, isSkybox} });
    status.wasSentToRenderer = true;
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
    eventsPtr->TriggerEvent(Event { EventType::DeleteModel, false, ModelDeletionPayload{modelID} });
}


void ModelCache::setAsSkybox(unsigned int modelID) {
    Model* model = getModel(modelID);
    if (model == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "MODELCACHE::setAsSkyBox()", "Model not found with ID " + std::to_string(modelID));
        return;
    }

    if (model->type != ModelType::CubePreset) {
        loggerPtr->addLog(LogLevel::WARNING, "MODELCACHE::setAsSkyBox()", "Model given is not of the type CubePreset");
        return;
    }

    if (model->getModelStatus().wasSentToRenderer == true) {
        eventsPtr->TriggerEvent(Event { EventType::DeleteModel, false, ModelDeletionPayload{modelID} });
        model->getModelStatus().wasSentToRenderer = false;
    }

    skyboxModelID = modelID;
    trySendingToRenderer(modelID);
}


void ModelCache::changeMeshMaterial(unsigned int modelID, unsigned int meshIdx, unsigned int materialID) {
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

    foundModel->setMeshMaterial(meshIdx, materialID);
    
    if (foundModel->getModelStatus().wasSentToRenderer == false) {
        trySendingToRenderer(modelID);
    }
    else {
        eventsPtr->TriggerEvent(Event { EventType::ModelMaterialChange, false, ModelMaterialChangePayload{modelID, meshIdx, materialID} });
    }
}


void ModelCache::changeModelMaterial(unsigned int modelID, unsigned int materialID) {
    Model* foundModel = getModel(modelID);
    if (foundModel == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE::changeModelMaterial()", "model not found from ID " + std::to_string(modelID));
        return;
    }
    foundModel->setModelMaterial(materialID);
    
    if (foundModel->getModelStatus().wasSentToRenderer == false) {
        trySendingToRenderer(modelID);
    } 
    else {
        eventsPtr->TriggerEvent(Event { EventType::ModelMaterialChange, false, ModelMaterialChangePayload{modelID, UINT_MAX, materialID} });
    }
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


bool ModelCache::validateNextID() {
    unsigned int numOfChecks = 0;
    while (modelIDMap.contains(nextModelID) == true) {
        if (numOfChecks > 1000) return false;
        
        nextModelID++;
        numOfChecks++;
    }
    return true;
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