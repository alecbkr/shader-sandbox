#include "ModelCache.hpp"

#include <memory>
#include "core/InspectorEngine.hpp"
#include "core/UniformRegistry.hpp"
#include "core/EventDispatcher.hpp"
#include "core/ShaderRegistry.hpp"
#include <algorithm>
#include <string>
#include "core/logging/Logger.hpp"
#include "object/ModelImporter.hpp"

ModelCache::ModelCache() {
    initialized = false;
    inspectorEngPtrSet = false;
    loggerPtr = nullptr;
    inspectorEngPtr = nullptr;
    eventsPtr = nullptr;
    shaderRegPtr = nullptr;
    uniformRegPtr = nullptr;
    modelImporterPtr = nullptr;
    modelCache.clear();
}

bool ModelCache::initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, ShaderRegistry* _shaderRegPtr, UniformRegistry* _uniformRegPtr, ModelImporter* _modelImporterPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Model Cache Initialization", "Model Cache was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    inspectorEngPtr = nullptr;;
    eventsPtr = _eventsPtr;
    shaderRegPtr = _shaderRegPtr;
    uniformRegPtr = _uniformRegPtr;
    modelImporterPtr = _modelImporterPtr;
    modelCache.clear();
    
    eventsPtr->Subscribe(EventType::ReloadShader, [this](const EventPayload& payload) -> bool {
        
        if (const auto* data = std::get_if<ReloadShaderPayload>(&payload)) {
            
            std::string programName = data->programName;
            for (auto& [ID, model] : modelIDMap) {
                if (model->getProgramID() == programName) {
                    model->setProgramID(programName);
                }
            }
            reorderByProgram();
            return true;
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
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODEL_CACHE", "createModel failed, ID already in use", std::to_string(nextModelID));
        return INVALID_MODEL;
    }
    
    std::unique_ptr<CustomModel> customModel = std::make_unique<CustomModel>(nextModelID, shaderRegPtr, loggerPtr);
    customModel->setMesh(vertices, indices, hasPos, hasNorms, hasUVs);
    Model* rawPointer = customModel.get();

    modelCache.push_back(std::move(customModel));
    modelIDMap.emplace(nextModelID, rawPointer);
    nextModelID++;
    return rawPointer->ID;
}


unsigned int ModelCache::createModel(std::string pathname) {

    if (modelIDMap.contains(nextModelID)) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODEL_CACHE", "createModel failed, ID already in use", std::to_string(nextModelID));
        return INVALID_MODEL;
    }

    std::unique_ptr<ImportedModel> importedModel = std::make_unique<ImportedModel>(nextModelID, pathname, modelImporterPtr, shaderRegPtr, loggerPtr);
    Model* rawPointer = importedModel.get();
    modelCache.push_back(std::move(importedModel));
    modelIDMap.emplace(nextModelID, rawPointer);
    nextModelID++;
    return rawPointer->ID;
}


void ModelCache::translateModel(unsigned int ID, glm::vec3 pos) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "OBJECT CACHE", "Model ID not found:", std::to_string(ID));
        return;
    }
    model->translate(pos);
}


void ModelCache::scaleModel(unsigned int ID, glm::vec3 scale) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "OBJECT CACHE", "Model ID not found:", std::to_string(ID));
        return;
    }
    model->rescale(scale);
}


void ModelCache::rotateModel(unsigned int ID, float angle, glm::vec3 axis) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "OBJECT CACHE", "Model ID not found:", std::to_string(ID));
        return;
    }
    model->rotate(angle, axis);
}


void ModelCache::setTexture(unsigned int ID, std::string pathname, std::string uniformName) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "OBJECT CACHE", "Model ID not found:", std::to_string(ID));
        return;
    }

    model->addTexture(pathname);
}


void ModelCache::setProgram(unsigned int ID, ShaderProgram &program) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "OBJECT CACHE", "Model ID not found:", std::to_string(ID));
        return;
    }

    model->setProgramID(program.name);
    reorderByProgram();
}


void ModelCache::renderModel(unsigned int ID, glm::mat4 perspective, glm::mat4 view) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "OBJECT CACHE", "Model ID not found:", std::to_string(ID));
        return;
    }

    ShaderProgram* currProgram = shaderRegPtr->getProgram(model->getProgramID());
    if (currProgram == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "OBJECT CACHE", "Shader ID not found:", model->getProgramID());
        return;
    }
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
        ShaderProgram* modelProgram = shaderRegPtr->getProgram(currModel->getProgramID());
        if (modelProgram == nullptr) continue;

        if (currProgram == nullptr || currProgram->ID != modelProgram->ID) {
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
        
        ShaderProgram* modelProgram = shaderRegPtr->getProgram(model->getProgramID());
        if (modelProgram == nullptr) continue;
        std::cout << modelProgram->ID << std::endl;
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
        [this](const std::unique_ptr<Model>& a, 
        const std::unique_ptr<Model>& b) 
    {  
        auto* progA = shaderRegPtr->getProgram(a->getProgramID());
        auto* progB = shaderRegPtr->getProgram(b->getProgramID());
    
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