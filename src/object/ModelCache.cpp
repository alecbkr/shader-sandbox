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
    inspectorEngPtrSet = false;
    loggerPtr = nullptr;
    inspectorEngPtr = nullptr;
    eventsPtr = nullptr;
    shaderRegPtr = nullptr;
    uniformRegPtr = nullptr;
    presetsPtr = nullptr;
    // modelCache.clear();
}

bool ModelCache::initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, ShaderRegistry* _shaderRegPtr, TextureCache* _textureCachePtr, UniformRegistry* _uniformRegPtr, InspectorEngine* _inspectorEngPtr, PresetAssets* _presetsPtr, MaterialCache* _materialCachePtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Model Cache Initialization", "Model Cache was already initialized.");
        return false;
    }
    loggerPtr       = _loggerPtr;
    inspectorEngPtr = _inspectorEngPtr;
    eventsPtr       = _eventsPtr;
    shaderRegPtr    = _shaderRegPtr;
    textureCachePtr = _textureCachePtr;
    uniformRegPtr   = _uniformRegPtr;
    presetsPtr      = _presetsPtr;
    materialCachePtr = _materialCachePtr;
    // modelImporterPtr = _modelImporterPtr;
   // modelCache.clear();
    
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
    inspectorEngPtr = nullptr;
    eventsPtr       = nullptr;
    shaderRegPtr    = nullptr;
    textureCachePtr = nullptr;
    // modelCache.clear();
    inspectorEngPtrSet = false;
    initialized = false;
}


unsigned int ModelCache::createCustom(
        std::vector<float> vertices, 
        std::vector<unsigned int> indices, 
        bool hasPos, bool hasNorms, bool hasUVs
) {

    if (modelIDMap.contains(nextModelID)) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODEL_CACHE", "createModel failed, ID already in use", std::to_string(nextModelID));
        return INVALID_MODEL;
    }
    
    std::unique_ptr<CustomModel> customModel = std::make_unique<CustomModel>(nextModelID, textureCachePtr, loggerPtr, materialCachePtr);
    customModel->setMesh(vertices, indices, hasPos, hasNorms, hasUVs);
    Model* rawPointer = customModel.get();

    modelIDMap.emplace(nextModelID, std::move(customModel));
    placeInQueue(nextModelID);
    
    nextModelID++;
    return rawPointer->ID;
}


unsigned int ModelCache::createImported(std::string model_path) {

    if (modelIDMap.contains(nextModelID)) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODEL_CACHE", "createModel failed, ID already in use", std::to_string(nextModelID));
        return INVALID_MODEL;
    }

    std::unique_ptr<ImportedModel> importedModel = std::make_unique<ImportedModel>(nextModelID, model_path, textureCachePtr, loggerPtr, materialCachePtr);
    Model* rawPointer = importedModel.get();
    modelIDMap.emplace(nextModelID, std::move(importedModel));
    // modelCache.push_back(rawPointer);
    placeInQueue(nextModelID); //adds primitives to model cache vectors

    nextModelID++;
    return rawPointer->ID;
}


unsigned int ModelCache::createPreset(MeshPreset preset) {
    MeshData presetData = presetsPtr->getPresetMesh(preset);
    std::unique_ptr<CustomModel> presetModel = std::make_unique<CustomModel>(nextModelID, textureCachePtr, loggerPtr, materialCachePtr);
    Model* rawPointer = presetModel.get();
    
    presetModel->setMesh(presetData.verts, presetData.indices, true, false, true);
    modelIDMap.emplace(presetModel->ID, std::move(presetModel));
    placeInQueue(nextModelID);
    nextModelID++;
    return rawPointer->ID;
}


unsigned int ModelCache::createSkybox(std::string cubemap_dir) {
    MeshData cube = presetsPtr->getPresetMesh(MeshPreset::CUBE);
    std::unique_ptr<CustomModel> skyboxModel = std::make_unique<CustomModel>(nextModelID, textureCachePtr, loggerPtr, materialCachePtr);
    Model* rawPointer = skyboxModel.get();

    skyboxModel->setMesh(cube.verts, cube.indices, true, false, true);
    skyboxModel->addTexture(cubemap_dir, TEX_CUBEMAP);
    skyboxModel->setMaterialType(skyboxModel->getAllMaterialIDs()[0], MaterialType::Skybox);
    modelIDMap.emplace(nextModelID, std::move(skyboxModel));
    placeInQueue(nextModelID);
    nextModelID++;
    return rawPointer->ID;
}


void ModelCache::renderAll(glm::mat4 perspective, glm::mat4 view, glm::vec3 camPos) {
    
    uniformRegPtr->registerSceneUniform({"projection", UniformType::Mat4, perspective});
    uniformRegPtr->registerSceneUniform({"view", UniformType::Mat4, view});

    // REGISTER INDIVIDUAL MODEL UNIFORMS
    for (auto& [modelID, model] : modelIDMap) {
        if (model->getMaterialType(model->getAllMaterialIDs()[0]) == MaterialType::Skybox) continue;
        uniformRegPtr->registerModelUniform(model->ID, {"model", UniformType::Mat4, model->getModelMatrix()});
    }

    // RENDER SKYBOX PRIMITIVE
    if (skyboxPrim != nullptr) {
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        inspectorEngPtr->applyAllUniformsForPrimitive(*skyboxPrim);
        getModel(skyboxPrim->ModelID)->renderPrimitive(0);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    // RENDER OPAQUE PRIMITIVES
    for (ModelPrimitive* prim : opaquePrims) {
        inspectorEngPtr->applyAllUniformsForPrimitive(*prim);
        // getModel(prim->ModelID)->renderPrimitive(prim->meshID);
        getModel(prim->ModelID)->renderPrimitive(prim->meshID); //modelInstanceCount[prim->ModelID]
    }

    // RENDER CUTOUT PRIMITIVES
    for (ModelPrimitive* prim : cutoutPrims) {
        inspectorEngPtr->applyAllUniformsForPrimitive(*prim);
        // getModel(prim->ModelID)->renderPrimitive(prim->meshID);
        getModel(prim->ModelID)->renderPrimitive(prim->meshID); //modelInstanceCount[prim->ModelID]
    }

    // RENDER TRANSLUCENT PRIMITIVES
    reorderTranslucentQueue(view);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    for (ModelPrimitive* prim : translucentPrims) {
        inspectorEngPtr->applyAllUniformsForPrimitive(*prim);
        getModel(prim->ModelID)->renderPrimitive(prim->meshID);
    }
    glDisable(GL_BLEND);
}


void ModelCache::renderModel(unsigned int modelID, glm::mat4 perspective, glm::mat4 view, glm::vec3 camPos) {
    Model* model = getModel(modelID);
    if (model == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "OBJECT CACHE", "Model ID not found:", std::to_string(modelID));
        return;
    }

    uniformRegPtr->registerSceneUniform({"projection", UniformType::Mat4, perspective});
    uniformRegPtr->registerSceneUniform({"view", UniformType::Mat4, view});
    // uniformRegPtr->registerUniform(model->ID, {"cameraPos", UniformType::Vec3, camPos});
    uniformRegPtr->registerModelUniform(modelID, {"model", UniformType::Mat4, model->getModelMatrix()});

    for (ModelPrimitive& prim : model->primitives) {
        inspectorEngPtr->applyAllUniformsForPrimitive(prim);
        model->renderPrimitive(prim.meshID);
    }
}


void ModelCache::renderPrimitive(unsigned int modelID, unsigned int meshID, glm::mat4 perspective, glm::mat4 view, glm::vec3 camPos) {
    Model* model = modelIDMap[modelID].get();
    if (model == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "OBJECT CACHE", "Model ID not found:", std::to_string(modelID));
        return;
    }
    uniformRegPtr->registerSceneUniform({"projection", UniformType::Mat4, perspective});
    uniformRegPtr->registerSceneUniform({"view", UniformType::Mat4, view});
    // uniformRegPtr->registerUniform(model->ID, {"cameraPos", UniformType::Vec3, camPos});
    uniformRegPtr->registerModelUniform(modelID, {"model", UniformType::Mat4, model->getModelMatrix()});
    
    model->renderPrimitive(meshID);
}


void ModelCache::deleteModel(unsigned int modelID) {
    Model* model = getModel(modelID);
    if (model == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "OBJECT CACHE", "Model ID not found:", std::to_string(modelID));
        return;
    }

    // REMOVE OPAQUE PRIMITIVES FROM CACHE
    auto newEnd = std::remove_if(opaquePrims.begin(), opaquePrims.end(), [&](ModelPrimitive* prim) {
        return prim->ModelID == modelID;
    });
    opaquePrims.erase(newEnd, opaquePrims.end());

    // REMOVE TRANSLUCENT PRIMITIVES FROM CACHE
    newEnd = std::remove_if(translucentPrims.begin(), translucentPrims.end(), [&](ModelPrimitive* prim) {
        return prim->ModelID == modelID;
    });
    translucentPrims.erase(newEnd, translucentPrims.end());

    modelIDMap.erase(modelID);
}


void ModelCache::printPrimRelations(unsigned int modelID) {
    Model* model = getModel(modelID);
    if (model == nullptr) return;

    for (unsigned int idx = 0; idx < model->primitives.size(); idx++) {
        std::cout << "index: " << idx;
        std::cout << " mesh: " << model->primitives[idx].meshID;
        std::cout << " material: " << model->primitives[idx].materialID << std::endl;
    }
}


void ModelCache::renderPrim(unsigned int modelID, unsigned int meshID, glm::mat4 perspective, glm::mat4 view) {
    Model* model = modelIDMap[modelID].get();

    uniformRegPtr->registerSceneUniform({"projection", UniformType::Mat4, perspective});
    uniformRegPtr->registerSceneUniform({"view", UniformType::Mat4, view});
    // uniformRegPtr->registerUniform(model->ID, {"cameraPos", UniformType::Vec3, camPos});
    uniformRegPtr->registerSceneUniform({"model", UniformType::Mat4, model->getModelMatrix()});

    model->renderPrimitive(meshID);
}


Model* ModelCache::getModel(unsigned int modelID) {
    auto it = modelIDMap.find(modelID);
    if (it == modelIDMap.end()) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE | getModel()", "modelID not found: ", std::to_string(modelID));
        return nullptr;
    }
    return it->second.get();
}


void ModelCache::setModelMaterialType(unsigned int modelID, unsigned int materialID, MaterialType type) {
    Model* model = getModel(modelID);
    if (model == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODELCACHE | setModelMaterialType()", "modelID not found");
        return;
    }

    MaterialType currMatType = model->getMaterialType(materialID);
    if (currMatType == type) {
        return;
    }

    std::vector<ModelPrimitive*> primsWithMaterialID;
    switch (currMatType) {
        case MaterialType::Opaque:
            for (size_t i = 0; i < opaquePrims.size(); i++) {
                if (opaquePrims[i]->ModelID == modelID && opaquePrims[i]->materialID == materialID) {
                    primsWithMaterialID.push_back(opaquePrims[i]);
                    opaquePrims.erase(opaquePrims.begin() + i);
                    i--;
                }
            }
            break;
        
        case MaterialType::Translucent:
            for (size_t i = 0; i < translucentPrims.size(); i++) {
                if (translucentPrims[i]->ModelID == modelID && translucentPrims[i]->materialID == materialID) {
                    primsWithMaterialID.push_back(translucentPrims[i]);
                    translucentPrims.erase(translucentPrims.begin() + i);
                    i--;
                }
            }
            break;

        case MaterialType::Cutout:
            for (size_t i = 0; i < cutoutPrims.size(); i++) {
                if (cutoutPrims[i]->ModelID == modelID && cutoutPrims[i]->materialID == materialID) {
                    primsWithMaterialID.push_back(cutoutPrims[i]);
                    cutoutPrims.erase(cutoutPrims.begin() + i);
                    i--;
                }
            }
            break;

        case MaterialType::Skybox:
            break;
    }

    model->setMaterialType(materialID, type);
    
    switch(type) {
        case MaterialType::Opaque: 
            opaquePrims.insert(opaquePrims.end(), primsWithMaterialID.begin(), primsWithMaterialID.end());
            break;

        case MaterialType::Translucent: 
            translucentPrims.insert(translucentPrims.end(), primsWithMaterialID.begin(), primsWithMaterialID.end());
            break;

        case MaterialType::Cutout: 
            cutoutPrims.insert(cutoutPrims.end(), primsWithMaterialID.begin(), primsWithMaterialID.end());
            break;

        case MaterialType::Skybox:
            break;
    }
}


void ModelCache::placeInQueue(unsigned int modelID) {
    Model* model = getModel(modelID);
    for (ModelPrimitive& prim : model->primitives) {
        switch (model->getMaterialType(prim.materialID)) {
            case MaterialType::Opaque:      opaquePrims.push_back(&prim); break;
            case MaterialType::Cutout:      opaquePrims.push_back(&prim); break;
            case MaterialType::Translucent: translucentPrims.push_back(&prim); break;
            case MaterialType::Skybox:      skyboxPrim = &prim; break;
        }
    }
}


int ModelCache::getNumberOfModels() {
    return modelIDMap.size();
}


void ModelCache::reorderTranslucentQueue(glm::mat4 viewMat) {
    for (ModelPrimitive* prim : translucentPrims) {
        Model* currModel = getModel(prim->ModelID);

        glm::vec4 viewPos = viewMat * glm::vec4(currModel->getPosition(), 1.0f);
        prim->depth = viewPos.z;
    }

    std::sort(translucentPrims.begin(), translucentPrims.end(),
        [](const ModelPrimitive* a, const ModelPrimitive* b) {
            return a->depth < b->depth;
        });

    // std::cout << 
}


// void ModelCache::reorderByProgram() {
//     std::sort(modelCache.begin(), modelCache.end(),
//         [](const Model* a, const Model* b) 
//     {  
//         auto* progA = shaderRegPtr->getProgram(a->getProgramID());
//         auto* progB = shaderRegPtr->getProgram(b->getProgramID());
    
//         // Both null equal
//         if (!progA && !progB)
//             return false;

//         // A is null goes after B
//         if (!progA)
//             return false;

//         // B is null A goes before B
//         if (!progB)
//             return true;

        // Both valid compare IDs
//         return progA->ID < progB->ID;
//     });
// }

void ModelCache::setInspectorEnginePtr(InspectorEngine* _inspectorEngPtr) {
    inspectorEngPtr = _inspectorEngPtr;
    inspectorEngPtrSet = true;
}
