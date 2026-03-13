#pragma once

#include <vector>

#include "Model.hpp"
#include "CustomModel.hpp"
#include "ImportedModel.hpp"
#include "ModelPrimitive.hpp"
#include "object/MaterialCache.hpp"
#include "presets/PresetAssets.hpp"

class Logger;
class InspectorEngine;
class EventDispatcher;
class ShaderRegistry;
class TextureCache;
class UniformRegistry;
class PresetAssets;

static constexpr unsigned int INVALID_MODEL = UINT_MAX;


class ModelCache {
    public:
        std::unordered_map<unsigned int, std::unique_ptr<Model>> modelIDMap; 
        
        ModelCache();
        bool initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, ShaderRegistry* _shaderRegPtr, TextureCache* _textureCachePtr, UniformRegistry* _uniformRegPtr, InspectorEngine* _inspectorEngPtr, PresetAssets* _presetsPtr, MaterialCache* _materialCachePtr);
        void shutdown();
       

        unsigned int createCustom(std::vector<float>, std::vector<unsigned int>, 
                                                        bool hasPos, bool hasNorms, bool hasUVs);
        unsigned int createImported(std::string model_path);
        unsigned int createPreset(MeshPreset type);
        unsigned int createSkybox(std::string cubemap_dir);

        void renderAll(glm::mat4 projection, glm::mat4 view, glm::vec3 camPos);
        void renderModel(unsigned int modelID, glm::mat4 projection, glm::mat4 view, glm::vec3 camPos);
        void renderPrimitive(unsigned int modelID, unsigned int meshID, glm::mat4 projection, glm::mat4 view, glm::vec3 camPos);
        void deleteModel(unsigned int modelID);
        Model* getModel(unsigned int modelID);
        std::vector<unsigned int> getModelIDs() const;
        void setModelMaterialType(unsigned int modelID, unsigned int materialID, MaterialType type);
        
        // DEBUG
        int getNumberOfModels();
        void printPrimRelations(unsigned int modelID);
        void renderPrim(unsigned int modelID, unsigned int meshID, glm::mat4 perspective, glm::mat4 view);
        void setInspectorEnginePtr(InspectorEngine* _inspectorEngPtr);

    private:
        unsigned int nextModelID = 0;
        std::vector<ModelPrimitive*> opaquePrims;
        std::vector<ModelPrimitive*> translucentPrims;
        std::vector<ModelPrimitive*> cutoutPrims;

        ModelPrimitive* skyboxPrim = nullptr;
        unsigned int skyboxID;

        // static void reorderByProgram();
        void reorderTranslucentQueue(glm::mat4 viewMat);
        void placeInQueue(unsigned int modelID);

        //SYSTEM FUNCTIONALITY
        bool initialized = false;
        bool inspectorEngPtrSet = false;
        Logger* loggerPtr                = nullptr;
        InspectorEngine* inspectorEngPtr = nullptr;
        EventDispatcher* eventsPtr       = nullptr;
        TextureCache* textureCachePtr    = nullptr;
        ShaderRegistry* shaderRegPtr     = nullptr;
        UniformRegistry* uniformRegPtr   = nullptr;
        PresetAssets* presetsPtr         = nullptr;
        MaterialCache* materialCachePtr  = nullptr;
};
