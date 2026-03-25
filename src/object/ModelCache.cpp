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
    
    // SETUP SKYBOX
    MeshData cubeMesh = presetsPtr->getPresetMesh(MeshPreset::CUBE);
    skyboxModel = new Model(UINT_MAX, std::string("SKYBOX"), ModelType::CubePreset);
    skyboxModel->addMeshByData(cubeMesh.verts, cubeMesh.indices, true, false, true);
    skyboxModel->setModelStateReady();

    
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
    delete skyboxModel;
    loggerPtr       = nullptr;
    eventsPtr       = nullptr;
    initialized = false;
}


unsigned int ModelCache::createPreset(ModelType type) {
    if (validateNextID() == false) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE | createPreset", "failed to validate next model ID on model creation");
        return INVALID_MODEL_ID;
    }

    if (type == ModelType::Imported) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE | createPreset", "invalid type call, imported model types are reserved for the importer");
        return INVALID_MODEL_ID;
    }

    unsigned int presetModelID = nextModelID;
    modelIDMap.emplace(presetModelID, std::make_unique<Model>(presetModelID, "preset", type));
    setupPreset(presetModelID, type);

    finalizeMesh(presetModelID);
    sendToRenderer(presetModelID);
    nextModelID++;
    return presetModelID;
}


//DO NOT CALL THIS OUTSIDE MODELIMPORTER
unsigned int ModelCache::createModelForImportSetup(std::string model_path) {
    if (validateNextID() == false) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE | createEmptyModel()", "failed to validate next model ID on model creation");
        return INVALID_MODEL_ID;
    }
    unsigned int emptyModelID = nextModelID;
    modelIDMap.emplace(nextModelID, std::make_unique<Model>(emptyModelID, model_path, ModelType::Imported));
    
    nextModelID++;
    return emptyModelID;
}


bool ModelCache::reserveModelID(unsigned int modelID, std::string model_path, ModelType type) {
    if (modelIDMap.contains(modelID)) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE | reserveModelID", "reservation failed. ModelCache already contains ID " + std::to_string(modelID));
        return false;
    }
    modelIDMap.emplace(modelID, std::make_unique<Model>(modelID, model_path, type));
    return true;
}


unsigned int ModelCache::createSkybox(std::string cubemap_dir) {
    // if (validateNextID() == false) {
    //     loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE", "failed to validate next model ID on model creation");
    //     return INVALID_MODEL_ID;
    // }
    // MeshData cube = presetsPtr->getPresetMesh(MeshPreset::CUBE);
    // std::unique_ptr<CustomModel> skyboxModel = std::make_unique<CustomModel>(nextModelID, std::string("SKYBOX_PRESET"));
    // Model* rawPointer = skyboxModel.get();

    // skyboxModel->addMesh(cube.verts, cube.indices, true, false, true);
    // // skyboxModel->addTexture(cubemap_dir, TEX_CUBEMAP);
    // // skyboxModel->setMaterialType(skyboxModel->getAllMaterialIDs()[0], MaterialType::Skybox);
    // modelIDMap.emplace(nextModelID, std::move(skyboxModel));
    // eventsPtr->TriggerEvent(Event { EventType::CreateModel, false, ModelCreationPayload{nextModelID} });
    // nextModelID++;
    // return rawPointer->ID;
    return 0; //dummy
}


void ModelCache::finalizeMesh(unsigned int modelID) {
    Model* model = getModel(modelID);
    if (model == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE | finalizeAnEmptyModel", "model not found: " + std::to_string(modelID));
        return;
    }

    bool result = model->setModelStateReady();
    if (result == false) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE | finalizeAnEmptyModel", "could not finalize model because its in error state, modelID: " + std::to_string(modelID));
        return;
    }
}


void ModelCache::sendToRenderer(unsigned int modelID) {
    eventsPtr->TriggerEvent(Event { EventType::CreateModel, false, ModelCreationPayload{modelID} });
}


void ModelCache::deleteModel(unsigned int modelID) {
    Model* model = getModel(modelID);
    if (model == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "OBJECT CACHE", "Model ID not found:", std::to_string(modelID));
        return;
    }
    modelIDMap.erase(modelID);
    eventsPtr->TriggerEvent(Event { EventType::DeleteModel, false, ModelDeletionPayload{modelID} });
}


void ModelCache::changeMeshMaterial(unsigned int modelID, unsigned int meshIdx, unsigned int materialID) {
    Model* foundModel = getModel(modelID);
    if (foundModel == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "ChangeMeshMaterial()", "model not found from ID " + std::to_string(modelID));
        return;
    }

    unsigned int numOfMeshes = foundModel->getNumberOfMeshes();
    if (meshIdx < 0 || numOfMeshes < meshIdx) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "ChangeMeshMaterial()", "mesh idx out of bounds " + std::to_string(meshIdx));
        return;
    }

    foundModel->setMeshMaterial(meshIdx, materialID);
    eventsPtr->TriggerEvent(Event { EventType::ModelMaterialChange, false, ModelMaterialChangePayload{modelID, meshIdx, materialID} });
}


void ModelCache::changeModelMaterial(unsigned int modelID, unsigned int materialID) {
    Model* foundModel = getModel(modelID);
    if (foundModel == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "ChangeMeshMaterial()", "model not found from ID " + std::to_string(modelID));
        return;
    }
    foundModel->setModelMaterial(materialID);
    eventsPtr->TriggerEvent(Event { EventType::ModelMaterialChange, false, ModelMaterialChangePayload{modelID, UINT_MAX, materialID} });
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
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE | getModel()", "modelID not found: ", std::to_string(modelID));
        return nullptr;
    }
    return it->second.get();
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


void ModelCache::setupPreset(unsigned int modelID, ModelType type) {
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