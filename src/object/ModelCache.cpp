#include "ModelCache.hpp"

#include <memory>
#include "../engine/Errorlog.hpp"
#include "core/UniformTypes.hpp"
#include <algorithm>
#include <iostream>
#include "core/logging/Logger.hpp"
#include "core/InspectorEngine.hpp"
#include "core/EventDispatcher.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/UniformRegistry.hpp"

ModelCache::ModelCache() {
    initialized = false;
    inspectorEngPtrSet = false;
    loggerPtr = nullptr;
    inspectorEngPtr = nullptr;
    eventsPtr = nullptr;
    shaderRegPtr = nullptr;
    uniformRegPtr = nullptr;
    modelCache.clear();
}

bool ModelCache::initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, ShaderRegistry* _shaderRegPtr, UniformRegistry* _uniformRegPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Model Cache Initialization", "Model Cache was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    inspectorEngPtr = nullptr;;
    eventsPtr = _eventsPtr;
    shaderRegPtr = _shaderRegPtr;
    uniformRegPtr = _uniformRegPtr;
    modelCache.clear();
    
    eventsPtr->Subscribe(EventType::ReloadShader, [this](const EventPayload& payload) -> bool {
        
        if (const auto* data = std::get_if<ReloadShaderPayload>(&payload)) {
            
            ShaderProgram* newProg = shaderRegPtr->getProgram(data->programName);
            if (newProg) {
                for (auto& [ID, model] : modelIDMap) {
                    if (model->getProgram()->name == data->programName) {
                        model->setProgram(*newProg);
                    }
                }
                reorderByProgram();
                return true;
            }
        }
        return false;
    });

    initialized = true;
    return true;
}

void ModelCache::shutdown() {
    loggerPtr = nullptr;
    inspectorEngPtr = nullptr;
    eventsPtr = nullptr;
    shaderRegPtr = nullptr;
    nextModelID = 0;
    modelCache.clear();
    inspectorEngPtrSet = false;
    initialized = false;
}


unsigned int ModelCache::createModel(
        std::vector<float> vertices, 
        std::vector<unsigned int> indices, 
        bool hasPos, bool hasNorms, bool hasUVs
) {

    if (modelIDMap.contains(nextModelID)) {
        ERRLOG.logEntry(EL_ERROR, "MODEL_CACHE", "createModel failed, ID already in use", nextModelID);
        return INVALID_MODEL;
    }
    
    std::unique_ptr<CustomModel> customModel = std::make_unique<CustomModel>(nextModelID);
    customModel->setMesh(vertices, indices, hasPos, hasNorms, hasUVs);
    Model* rawPointer = customModel.get();

    modelCache.push_back(std::move(customModel));
    modelIDMap.emplace(nextModelID, rawPointer);
    nextModelID++;
    return rawPointer->ID;
}


unsigned int ModelCache::createModel(std::string pathname) {

    if (modelIDMap.contains(nextModelID)) {
        ERRLOG.logEntry(EL_ERROR, "MODEL_CACHE", "createModel failed, ID already in use", nextModelID);
        return INVALID_MODEL;
    }

    std::unique_ptr<ImportedModel> importedModel = std::make_unique<ImportedModel>(nextModelID, pathname);
    Model* rawPointer = importedModel.get();
    modelCache.push_back(std::move(importedModel));
    modelIDMap.emplace(nextModelID, rawPointer);
    nextModelID++;
    return rawPointer->ID;
}


void ModelCache::translateModel(unsigned int ID, glm::vec3 pos) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Model ID not found:", ID);
        return;
    }
    model->translate(pos);
}


void ModelCache::scaleModel(unsigned int ID, glm::vec3 scale) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Model ID not found:", ID);
        return;
    }
    model->rescale(scale);
}


void ModelCache::rotateModel(unsigned int ID, float angle, glm::vec3 axis) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Model ID not found:", ID);
        return;
    }
    model->rotate(angle, axis);
}


void ModelCache::setTexture(unsigned int ID, std::string pathname, std::string uniformName) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Model ID not found:", ID);
        return;
    }

    model->addTexture(pathname);
}


void ModelCache::setProgram(unsigned int ID, ShaderProgram &program) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Model ID not found:", ID);
        return;
    }

    model->setProgram(program);
    reorderByProgram();
}


void ModelCache::renderModel(unsigned int ID, glm::mat4 perspective, glm::mat4 view) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Model ID not found:", ID);
        return;
    }

    ShaderProgram* currProgram = model->getProgram();
    currProgram->use();
    currProgram->setUniform_mat4float("projection", perspective);
    currProgram->setUniform_mat4float("view", view);
    currProgram->setUniform_mat4float("model", model->modelM);
    model->renderModel();
}


void ModelCache::renderAll(glm::mat4 perspective, glm::mat4 view) {
    if (!inspectorEngPtrSet) {
        if (initialized) loggerPtr->addLog(LogLevel::WARNING, "Model Cache renderAll", "Inspector Engine pointer has not been set yet.");
        return;
    }
    ShaderProgram *currProgram = nullptr;
    for (auto& currModel : modelCache) {
        ShaderProgram* modelProgram = currModel->getProgram();
        if (currModel->getProgram() == nullptr) continue;

        if (currProgram == nullptr || currProgram->ID != currModel.get()->getProgram()->ID) {
            currProgram = modelProgram;
            currProgram->use();
        }
        
        uniformRegPtr->registerUniform(currModel->ID, {"projection", UniformType::Mat4, perspective});
        uniformRegPtr->registerUniform(currModel->ID, {"view", UniformType::Mat4, view});
        uniformRegPtr->registerUniform(currModel->ID, {"model", UniformType::Mat4, currModel->modelM});

        inspectorEngPtr->applyAllUniformsForObject(currModel->ID); //TODO InspectorEngine
        currModel->renderModel();
    }
}


void ModelCache::printOrder() {
    for (auto& model : modelCache) {
        std::cout << model->getProgram()->ID << std::endl;
    }
}


Model* ModelCache::getModel(unsigned int ID) {
    
    auto it = modelIDMap.find(ID);
    if (it == modelIDMap.end())
        return nullptr;

    return it->second.get();
}


int ModelCache::getNumberOfModels() {
    return modelIDMap.size();
}


void ModelCache::reorderByProgram() {
    std::sort(modelCache.begin(), modelCache.end(),
        [](const std::unique_ptr<Model>& a, 
        const std::unique_ptr<Model>& b) 
    {  
        auto* progA = a->getProgram();
        auto* progB = b->getProgram();
    
        // Both null equal
        if (!progA && !progB)
            return false;

        // A is null goes after B
        if (!progA)
            return false;

        // B is null A goes before B
        if (!progB)
            return true;

        // Both valid compare IDs
        return progA->ID < progB->ID;
    });
}

void ModelCache::setInspectorEnginePtr(InspectorEngine* _inspectorEngPtr) {
    inspectorEngPtr = _inspectorEngPtr;
    inspectorEngPtrSet = true;
}