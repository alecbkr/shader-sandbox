#include "ModelCache.hpp"
#include <memory>
#include "../engine/Errorlog.hpp"
#include "core/InspectorEngine.hpp"
#include "core/UniformRegistry.hpp"
#include "core/UniformTypes.hpp"
#include <algorithm>
#include <iostream>

std::unordered_map<unsigned int, std::unique_ptr<Model>> ModelCache::modelIDMap;
std::vector<std::unique_ptr<Model>> ModelCache::modelCache; 


void ModelCache::createModel(const unsigned int ID, std::vector<float> vertices, std::vector<unsigned int> indices, 
                             bool hasPos, bool hasNorms, bool hasUVs) {

    if (modelIDMap.contains(ID)) {
        ERRLOG.logEntry(EL_WARNING, "MODEL_CACHE", "createModel failed, ID already in use", ID);
        return;
    }
    
    std::unique_ptr<CustomModel> customModel = std::make_unique<CustomModel>(ID);
    customModel->setMesh(vertices, indices, hasPos, hasNorms, hasUVs);
    Model* rawPointer = customModel.get();

    modelCache.push_back(std::move(customModel));
    modelIDMap.emplace(ID, rawPointer);
}


void ModelCache::createModel(const unsigned int ID, std::string pathname) {

    if (modelIDMap.contains(ID)) {
        ERRLOG.logEntry(EL_WARNING, "MODEL_CACHE", "createModel failed, ID already in use", ID);
        return;
    }

    std::unique_ptr<ImportedModel> importedModel = std::make_unique<ImportedModel>(ID, pathname);
    Model* rawPointer = importedModel.get();

    modelCache.push_back(std::move(importedModel));
    modelIDMap.emplace(ID, rawPointer);
}


void ModelCache::translateModel(const int ID, glm::vec3 pos) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Model ID not found:", ID);
        return;
    }
    model->translate(pos);
}


void ModelCache::scaleModel(const int ID, glm::vec3 scale) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Model ID not found:", ID);
        return;
    }
    model->rescale(scale);
}


void ModelCache::rotateModel(const int ID, float angle, glm::vec3 axis) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Model ID not found:", ID);
        return;
    }
    model->rotate(angle, axis);
}


void ModelCache::setTexture(const int ID, std::string pathname, std::string uniformName) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Model ID not found:", ID);
        return;
    }

    model->addTexture(pathname);
}


void ModelCache::setProgram(const int ID, ShaderProgram &program) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Model ID not found:", ID);
        return;
    }

    model->setProgram(program);
    reorderByProgram();
}


void ModelCache::renderModel(const int ID, glm::mat4 perspective, glm::mat4 view) {
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
        if (currProgram == nullptr || currProgram->ID != currModel.get()->getProgram()->ID) {
            currProgram = currModel.get()->getProgram();
            currProgram->use();
        }
        

        // Uniform uPerspective{
        //     .name = "projection",
        //     .type = UniformType::Mat4,
        //     .value = perspective
        // };
        // Uniform uView{
        //     .name = "view",
        //     .type = UniformType::Mat4,
        //     .value = view
        // };
        // Uniform uModel{
        //     .name = "model",
        //     .type = UniformType::Mat4,
        //     .value = currObj.get()->getModelM()
        // };
        // UNIFORM_REGISTRY.registerUniform(currObj.get()->ID, uPerspective);
        // UNIFORM_REGISTRY.registerUniform(currObj.get()->ID, uView);
        // UNIFORM_REGISTRY.registerUniform(currObj.get()->ID, uModel);
        // InspectorEngine::applyAllUniformsForObject(currObj.get()->ID);
        currProgram->setUniform_mat4float("projection", perspective);
        currProgram->setUniform_mat4float("view", view);
        currProgram->setUniform_mat4float("model", currModel->modelM);
        currModel.get()->renderModel();
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


void ModelCache::reorderByProgram() {
    std::sort(modelCache.begin(), modelCache.end(),
              [](const std::unique_ptr<Model>& a, const std::unique_ptr<Model>& b) {
                  return a.get()->getProgram()->ID < b.get()->getProgram()->ID;
              });
}
