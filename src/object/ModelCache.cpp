#include "ModelCache.hpp"

#include <memory>
#include "../engine/Errorlog.hpp"
#include "core/InspectorEngine.hpp"
#include "core/UniformRegistry.hpp"
#include "core/UniformTypes.hpp"
#include "core/EventDispatcher.hpp"
#include "core/ShaderRegistry.hpp"
#include <algorithm>
#include <iostream>

std::unordered_map<unsigned int, std::unique_ptr<Model>> ModelCache::modelIDMap;
std::vector<std::unique_ptr<Model>> ModelCache::modelCache; 
unsigned int ModelCache::nextModelID = 0;


bool ModelCache::initialize() {
    EventDispatcher::Subscribe(EventType::ReloadShader, [](const EventPayload& payload) -> bool {
        
        if (const auto* data = std::get_if<ReloadShaderPayload>(&payload)) {
            
            ShaderProgram* newProg = ShaderRegistry::getProgram(data->programName);
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

    return true;
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
    ShaderProgram *currProgram = nullptr;
    for (auto& currModel : modelCache) {
        ShaderProgram* modelProgram = currModel->getProgram();
        if (currModel->getProgram() == nullptr) continue;

        if (currProgram == nullptr || currProgram->ID != currModel.get()->getProgram()->ID) {
            currProgram = modelProgram;
            currProgram->use();
        }
        
        UNIFORM_REGISTRY.registerUniform(currModel->ID, {"projection", UniformType::Mat4, perspective});
        UNIFORM_REGISTRY.registerUniform(currModel->ID, {"view", UniformType::Mat4, view});
        UNIFORM_REGISTRY.registerUniform(currModel->ID, {"model", UniformType::Mat4, currModel->modelM});

        InspectorEngine::applyAllUniformsForObject(currModel->ID); //TODO InspectorEngine
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
