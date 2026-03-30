#include "Renderer.hpp"

#include "core/logging/Logger.hpp"
#include "core/EventDispatcher.hpp"
#include "ModelCache.hpp"
#include "MaterialCache.hpp"
#include "texture/TextureCache.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/UniformRegistry.hpp"
#include "core/InspectorEngine.hpp"

#include <algorithm>

#include <iostream>


Renderer::Renderer() {}


bool Renderer::initialize(
    Logger* _loggerPtr, EventDispatcher* _eventsPtr, ModelCache* _modelCachePtr, 
    MaterialCache* _materialCachePtr, TextureCache* _textureCachePtr, ShaderRegistry* _shaderRegPtr,
    UniformRegistry* _uniformRegPtr, InspectorEngine* _inspectorEngPtr
) {
    loggerPtr        = _loggerPtr;
    eventsPtr        = _eventsPtr;
    modelCachePtr    = _modelCachePtr;
    materialCachePtr = _materialCachePtr;
    textureCachePtr  = _textureCachePtr;
    shaderRegPtr     = _shaderRegPtr;
    uniformRegPtr    = _uniformRegPtr;
    inspectorEngPtr  = _inspectorEngPtr;

    // GRAB PRELOADED MODELS
    // Model* skybox = modelCachePtr->getSkybox();
    // primitiveIDMap.emplace(
    //     nextPrimitiveID,
    //     Primitive {
    //         .modelID = data->modelID, 
    //         .meshIdx = meshInstance.meshIdx, 
    //         .materialID = meshInstance.materialID, 
    //         .depth = 0,
    //         .queuetype = queueType
    //     }
    // );

    eventsPtr->Subscribe(EventType::CreateModel, [this](const EventPayload& payload) -> bool {
        if (const auto* data = std::get_if<ModelCreationPayload>(&payload)) {
            
            Model* newModel = modelCachePtr->getModel(data->modelID);
            for (const MeshInstance& meshInstance : newModel->getMeshInstances()) {
                if (validateNextID() == false) {
                    loggerPtr->addLog(LogLevel::LOG_ERROR, "RENDERER::Event", "failed to validate next primitive ID on model creation");
                    break;
                }

                QueueType queueType;
                if (data->isSkyBox == true) {
                    queueType = Skybox;
                }
                else {
                    MaterialType materialType = materialCachePtr->getMaterial(meshInstance.materialID)->getMaterialType();
                    switch (materialType) {
                        case MaterialType::Opaque:      queueType = Opaque;      break;
                        case MaterialType::Cutout:      queueType = Cutout;      break;
                        case MaterialType::Translucent: queueType = Translucent; break;
                    }   
                }
                
                primitiveIDMap.emplace(
                    nextPrimitiveID, 
                    Primitive{
                        .modelID = data->modelID, 
                        .meshIdx = meshInstance.meshIdx, 
                        .materialID = meshInstance.materialID, 
                        .depth = 0,
                        .queuetype = queueType
                    });
                
                placeInQueue(nextPrimitiveID, queueType);

                nextPrimitiveID++;
            }
            return true;
        }
        return false;
    });

    eventsPtr->Subscribe(EventType::DeleteModel, [this](const EventPayload& payload) -> bool {
        if (const auto* data = std::get_if<ModelDeletionPayload>(&payload)) {
            
            for (auto iter = primitiveIDMap.begin(); iter != primitiveIDMap.end(); ) {
                if (iter->second.modelID == data->modelID) {
                    removeFromQueue(iter->first, iter->second.queuetype);
                    iter = primitiveIDMap.erase(iter);
                }
                else {
                    iter++;
                }
            
            }
            return true;
        }
        return false;
    });

    eventsPtr->Subscribe(EventType::ModelMaterialChange, [this](const EventPayload& payload) -> bool {
        if (const auto* data = std::get_if<ModelMaterialChangePayload>(&payload)) {

            for (auto& [primitiveID, primitive] : primitiveIDMap) {
                if (primitive.modelID == data->modelID && (data->meshIdx == UINT_MAX || primitive.meshIdx == data->meshIdx)) {

                    primitive.materialID = data->materialID;
                    QueueType newHostQueueType;
                    switch (materialCachePtr->getMaterial(data->materialID)->getMaterialType()) {
                        case MaterialType::Opaque:      newHostQueueType = Opaque;      break;
                        case MaterialType::Cutout:      newHostQueueType = Cutout;      break;
                        case MaterialType::Translucent: newHostQueueType = Translucent; break;
                    } 
                    QueueType oldHostQueueType = primitive.queuetype;
                    if (newHostQueueType != oldHostQueueType) {
                        primitive.queuetype = newHostQueueType;
                        removeFromQueue(primitiveID, oldHostQueueType);
                        placeInQueue(primitiveID, newHostQueueType);
                    }
                }
            }
            return true;
        }
        return false;
    });

    eventsPtr->Subscribe(EventType::MaterialTypeChange, [this](const EventPayload& payload) -> bool {
        if (const auto* data = std::get_if<MaterialTypeChangePayload>(&payload)) {

            for (auto iter = primitiveIDMap.begin(); iter != primitiveIDMap.end(); ) {
                if (iter->second.materialID == data->materialID) {
                    QueueType newHostQueueType;
                    switch (data->newType) {
                        case MaterialType::Opaque:      newHostQueueType = Opaque;      break;
                        case MaterialType::Cutout:      newHostQueueType = Cutout;      break;
                        case MaterialType::Translucent: newHostQueueType = Translucent; break;
                    }
                    unsigned int primitiveID = iter->first;
                    QueueType oldHostQueueType = iter->second.queuetype;
                    removeFromQueue(primitiveID, oldHostQueueType);
                    placeInQueue(primitiveID, newHostQueueType);
                }
                else {
                    iter++;
                }
            }
            return true;
        }
        return false;
    });

    return true;
}


void Renderer::renderAll(glm::mat4 perspective, glm::mat4 view, glm::vec3 camPos) {
    uniformRegPtr->registerSceneUniform({"projection", UniformType::Mat4, perspective});
    uniformRegPtr->registerSceneUniform({"view", UniformType::Mat4, view});

    unsigned int skyboxModelID = modelCachePtr->getSkyboxModelID();
    for (auto& model : modelCachePtr->getAllModels()) {
        if (model->ID == skyboxModelID) continue;
        uniformRegPtr->registerModelUniform(model->ID, {"model", UniformType::Mat4, model->getModelMatrix()});
    }
    
    renderSkybox();
    renderOpaquePrimitives();
    renderCutoutPrimitives();
    reorderTranslucentPrimitives(view);
    renderTranslucentPrimitives();
}


void Renderer::renderSkybox() {
    if (skyboxPrimID == UINT_MAX) return;

    if (validatePrimitive(skyboxPrimID) == false) {
        // primitiveIDMap.erase(skyboxPrimID);
        skyboxPrimID = UINT_MAX;
        return;
    }
    
    Primitive* skyboxPrimitive = &primitiveIDMap.at(skyboxPrimID);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    bindProgram(skyboxPrimitive->materialID);
    inspectorEngPtr->applyAllUniformsForPrimitive(skyboxPrimitive->modelID, skyboxPrimitive->meshIdx, skyboxPrimitive->materialID);
    bindTextures(skyboxPrimitive->materialID);
    drawMesh(skyboxPrimitive->modelID, skyboxPrimitive->meshIdx);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}


void Renderer::renderOpaquePrimitives() {
    for (unsigned int primitiveID : opaquePrimIDs) {
        
        if (validatePrimitive(primitiveID) == false) {
            // removeFromQueue(primitiveID, QueueType::Opaque);
            continue;
        }

        Primitive* currPrimitive = &primitiveIDMap.at(primitiveID);
        bindProgram(currPrimitive->materialID);
        inspectorEngPtr->applyAllUniformsForPrimitive(currPrimitive->modelID, currPrimitive->meshIdx, currPrimitive->materialID);
        bindTextures(currPrimitive->materialID);
        drawMesh(currPrimitive->modelID, currPrimitive->meshIdx);
    }
}


void Renderer::renderCutoutPrimitives() {
    for (unsigned int primitiveID : cutoutPrimIDs) {

        if (validatePrimitive(primitiveID) == false) {
            // removeFromQueue(primitiveID, QueueType::Cutout);
            continue;
        }

        Primitive* currPrimitive = &primitiveIDMap.at(primitiveID);
        bindProgram(currPrimitive->materialID);
        inspectorEngPtr->applyAllUniformsForPrimitive(currPrimitive->modelID, currPrimitive->meshIdx, currPrimitive->materialID);
        bindTextures(currPrimitive->materialID);
        drawMesh(currPrimitive->modelID, currPrimitive->meshIdx);
    }
}


void Renderer::renderTranslucentPrimitives() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    for (unsigned int primitiveID : translucentPrimIDs) {

        if (validatePrimitive(primitiveID) == false) {
            // removeFromQueue(primitiveID, QueueType::Translucent);
            continue;
        }

        Primitive* currPrimitive = &primitiveIDMap.at(primitiveID);
        bindProgram(currPrimitive->materialID);
        inspectorEngPtr->applyAllUniformsForPrimitive(currPrimitive->modelID, currPrimitive->meshIdx, currPrimitive->materialID);
        bindTextures(currPrimitive->materialID);
        drawMesh(currPrimitive->modelID, currPrimitive->meshIdx);
    }
    glDisable(GL_BLEND);
}


void Renderer::reorderTranslucentPrimitives(glm::mat4 viewMat) {
    for (unsigned int primitiveID : translucentPrimIDs) {
    
        Primitive* currPrimitive = &primitiveIDMap.at(primitiveID);
        Model* currModel = modelCachePtr->getModel(currPrimitive->modelID);
        glm::vec4 viewPos = viewMat * glm::vec4(currModel->getPosition(), 1.0f);
        currPrimitive->depth = viewPos.z;
    }

    std::sort(translucentPrimIDs.begin(), translucentPrimIDs.end(),
        [this](unsigned int a, unsigned int b) {
            const Primitive& primitiveA = primitiveIDMap.at(a);
            const Primitive& primitiveB = primitiveIDMap.at(b);

            return primitiveA.depth < primitiveB.depth;
        }
    );
}


void Renderer::bindTextures(unsigned int materialID) {
    Material* foundMaterial = materialCachePtr->getMaterial(materialID);

    const auto& textureIDs = foundMaterial->getMaterialTextureIDs();
    if (textureIDs.empty() == false) {
        unsigned int texUnit = 0;
        for (unsigned int textureID : textureIDs) {
            textureCachePtr->bindTexture(textureID, texUnit);
            texUnit++;
        }
    }
    else {
        textureCachePtr->bindDefault(0);
    }
}


void Renderer::bindProgram(unsigned int materialID) {
    Material* foundMaterial = materialCachePtr->getMaterial(materialID);  
    ShaderProgram* program = shaderRegPtr->getProgram(foundMaterial->getProgramID());
    program->use();
}


void Renderer::drawMesh(unsigned int modelID, unsigned int meshIdx) {
    Model* foundModel = modelCachePtr->getModel(modelID);
    foundModel->drawMesh(meshIdx);
}


bool Renderer::validateNextID() {
    unsigned int numOfChecks = 0;
    while (primitiveIDMap.contains(nextPrimitiveID) == true) {
        if (numOfChecks > 1000) return false;
        
        nextPrimitiveID++;
        numOfChecks++;
    }
    return true;
}


void Renderer::placeInQueue(unsigned int primitiveID, QueueType queueType) {
    std::vector<unsigned int>* hostQueue;
    switch (queueType) {
        case Opaque:      hostQueue = &opaquePrimIDs;      break;
        case Cutout:      hostQueue = &cutoutPrimIDs;      break;
        case Translucent: hostQueue = &translucentPrimIDs; break;
        case Skybox:
            skyboxPrimID = primitiveID;
            return;
    }
    hostQueue->push_back(primitiveID);
}


void Renderer::removeFromQueue(unsigned int primitiveIDToDelete, QueueType queueType) {
    std::vector<unsigned int>* hostQueue;
    switch (queueType) {
        case Opaque:      hostQueue = &opaquePrimIDs;      break;
        case Cutout:      hostQueue = &cutoutPrimIDs;      break;
        case Translucent: hostQueue = &translucentPrimIDs; break;
        case Skybox:
            skyboxPrimID = UINT_MAX;
            return;
    }
    for (auto iter = hostQueue->begin(); iter != hostQueue->end(); ) {
        if (*iter == primitiveIDToDelete) {
            hostQueue->erase(iter);
            return;
        }
        else {
            iter++;
        }
    }
}


bool Renderer::validatePrimitive(unsigned int primitiveID) {
    if (primitiveIDMap.contains(primitiveID) == false) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "RENDERER::validatePrimitive", "primitive not found in map with ID " + std::to_string(primitiveID));
        return false;
    }

    Primitive& primitive = primitiveIDMap.at(primitiveID);
    bool result = true;
    std::string feedback = "";

    Model* foundModel = modelCachePtr->getModel(primitive.modelID);
    if (foundModel == nullptr) {
        feedback += "\nmodel not found with ID " + std::to_string(primitive.modelID);
        result = false;
    }
    else if (primitive.meshIdx >= foundModel->getNumberOfMeshes()) {
        feedback += "\nmesh with Index " + std::to_string(primitive.meshIdx) + " out of bounds for model " + std::to_string(primitive.modelID);
        result = false;
    }

    Material* foundMaterial = materialCachePtr->getMaterial(primitive.materialID);  
    if (foundMaterial == nullptr) {
        std::cout << "MATERR" << std::endl; //TEMPPRINT
        feedback += "\nmaterial not found with ID: " + std::to_string(primitive.materialID) + ". Removing materialID from model";
        result = false;
    }
    else if (shaderRegPtr->getProgram(foundMaterial->getProgramID()) == nullptr) {
        feedback += "\nshader program not found: " + foundMaterial->getProgramID();
        result = false;
    }

    if (result == false) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "RENDERER::validatePrimitive()", feedback);
    }
    return result;
}