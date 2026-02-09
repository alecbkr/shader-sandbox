#include "ModelCache.hpp"

#include <memory>
#include "core/InspectorEngine.hpp"
#include "core/UniformRegistry.hpp"
#include "core/EventDispatcher.hpp"
#include "core/ShaderRegistry.hpp"
#include <algorithm>
#include <string>
#include "core/logging/Logger.hpp"
#include "../presets/PresetAssets.hpp"
#include "ModelTypes.hpp"

std::unordered_map<unsigned int, std::unique_ptr<Model>> ModelCache::modelIDMap;
std::vector<Model*> ModelCache::modelCache; 
unsigned int ModelCache::nextModelID = 0;


bool ModelCache::initialize() {
    EventDispatcher::Subscribe(EventType::ReloadShader, [](const EventPayload& payload) -> bool {
        
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

    return true;
}


unsigned int ModelCache::createModel(
        std::vector<float> vertices, 
        std::vector<unsigned int> indices, 
        bool hasPos, bool hasNorms, bool hasUVs
) {

    if (modelIDMap.contains(nextModelID)) {
        Logger::addLog(LogLevel::ERROR, "MODEL_CACHE", "createModel failed, ID already in use", std::to_string(nextModelID));
        return INVALID_MODEL;
    }
    
    std::unique_ptr<CustomModel> customModel = std::make_unique<CustomModel>(nextModelID, T_MODEL);
    customModel->setMesh(vertices, indices, hasPos, hasNorms, hasUVs);
    Model* rawPointer = customModel.get();

    modelIDMap.emplace(nextModelID, std::move(customModel));
    modelCache.push_back(rawPointer);
    
    nextModelID++;
    return rawPointer->ID;
}


unsigned int ModelCache::createModel(std::string pathname) {

    if (modelIDMap.contains(nextModelID)) {
        Logger::addLog(LogLevel::ERROR, "MODEL_CACHE", "createModel failed, ID already in use", std::to_string(nextModelID));
        return INVALID_MODEL;
    }
    std::unique_ptr<ImportedModel> importedModel = std::make_unique<ImportedModel>(nextModelID, pathname);
    Model* rawPointer = importedModel.get();
    modelIDMap.emplace(nextModelID, std::move(importedModel));
    modelCache.push_back(rawPointer);
   
    nextModelID++;
    return rawPointer->ID;
}


unsigned int ModelCache::createSkybox(std::string cubemapDir) {
    MeshData cube = PresetAssets::getPresetMesh(MeshPreset::CUBE);
    std::unique_ptr<CustomModel> cubeModel = std::make_unique<CustomModel>(nextModelID, T_SKYBOX);
    Model* rawPointer = cubeModel.get();

    cubeModel->setMesh(cube.verts, cube.indices, true, false, true);
    cubeModel->addCubeMap(cubemapDir);
    modelIDMap.emplace(nextModelID, std::move(cubeModel));
    modelCache.push_back(rawPointer);
    nextModelID++;
    return rawPointer->ID;
}


void ModelCache::translateModel(unsigned int ID, glm::vec3 pos) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        Logger::addLog(LogLevel::WARNING, "OBJECT CACHE", "Model ID not found:", std::to_string(ID));
        return;
    }
    model->translate(pos);
}


void ModelCache::scaleModel(unsigned int ID, glm::vec3 scale) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        Logger::addLog(LogLevel::WARNING, "OBJECT CACHE", "Model ID not found:", std::to_string(ID));
        return;
    }
    model->rescale(scale);
}


void ModelCache::rotateModel(unsigned int ID, float angle, glm::vec3 axis) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        Logger::addLog(LogLevel::WARNING, "OBJECT CACHE", "Model ID not found:", std::to_string(ID));
        return;
    }
    model->rotate(angle, axis);
}


void ModelCache::setTexture(unsigned int ID, std::string pathname, std::string uniformName) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        Logger::addLog(LogLevel::WARNING, "OBJECT CACHE", "Model ID not found:", std::to_string(ID));
        return;
    }
    model->addTexture(pathname);
}


void ModelCache::setProgram(unsigned int ID, ShaderProgram &program) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        Logger::addLog(LogLevel::WARNING, "OBJECT CACHE", "Model ID not found:", std::to_string(ID));
        return;
    }

    model->setProgramID(program.name);
    reorderByProgram();
}


void ModelCache::renderModel(unsigned int ID, glm::mat4 perspective, glm::mat4 view, glm::vec3 camPos) {
    Model* model = getModel(ID);
    if (model == nullptr) {
        Logger::addLog(LogLevel::WARNING, "OBJECT CACHE", "Model ID not found:", std::to_string(ID));
        return;
    }

    ShaderProgram* currProgram = ShaderRegistry::getProgram(model->getProgramID());
    if (currProgram == nullptr) {
        Logger::addLog(LogLevel::WARNING, "OBJECT CACHE", "Shader ID not found:", model->getProgramID());
        return;
    }

    currProgram->use();
    switch (model->type) {
        case T_MODEL:
            currProgram->setUniform_mat4float("projection", perspective);
            currProgram->setUniform_mat4float("view", view);
            currProgram->setUniform_mat4float("model", model->modelM);
            break;
        
        case T_SKYBOX:
            currProgram->setUniform_mat4float("projection", perspective);
            currProgram->setUniform_mat4float("view", glm::mat4(glm::mat3(view)));
            break;
    }
    model->renderModel();
}


void ModelCache::renderAll(glm::mat4 perspective, glm::mat4 view, glm::vec3 camPos) {
    ShaderProgram *currProgram = nullptr;
    for (auto& currModel : modelCache) {
        ShaderProgram* modelProgram = ShaderRegistry::getProgram(currModel->getProgramID());
        if (modelProgram == nullptr) continue;

        if (currProgram == nullptr || currProgram->ID != modelProgram->ID) {
            currProgram = modelProgram;
            currProgram->use();
        }

        switch (currModel->type) {
            case T_MODEL:
                UNIFORM_REGISTRY.registerUniform(currModel->ID, {"projection", UniformType::Mat4, perspective});
                UNIFORM_REGISTRY.registerUniform(currModel->ID, {"view", UniformType::Mat4, view});
                UNIFORM_REGISTRY.registerUniform(currModel->ID, {"model", UniformType::Mat4, currModel->modelM});
                UNIFORM_REGISTRY.registerUniform(currModel->ID, {"cameraPos", UniformType::Vec3, camPos});
                break;
            
            case T_SKYBOX:
                UNIFORM_REGISTRY.registerUniform(currModel->ID, {"projection", UniformType::Mat4, perspective});
                UNIFORM_REGISTRY.registerUniform(currModel->ID, {"view", UniformType::Mat4, glm::mat4(glm::mat3(view))});
                break;
        }
        InspectorEngine::applyAllUniformsForObject(currModel->ID); //TODO InspectorEngine
        currModel->renderModel();
    }
}


void ModelCache::printOrder() {
    for (auto& model : modelCache) {
        ShaderProgram* modelProgram = ShaderRegistry::getProgram(model->getProgramID());
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
        [](const Model* a, const Model* b) 
    {  
        auto* progA = ShaderRegistry::getProgram(a->getProgramID());
        auto* progB = ShaderRegistry::getProgram(b->getProgramID());
    
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
