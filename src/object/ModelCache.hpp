#pragma once

#include <vector>

#include "Model.hpp"
#include "CustomModel.hpp"
#include "ImportedModel.hpp"
#include "ModelPrimitive.hpp"
#include "presets/PresetAssets.hpp"

static constexpr unsigned int INVALID_MODEL = UINT_MAX;


class ModelCache {
    public:
        static std::unordered_map<unsigned int, std::unique_ptr<Model>> modelIDMap; 

        static bool initialize();
        ModelCache() = delete;

        static unsigned int createCustom(std::vector<float>, std::vector<unsigned int>, 
                                                        bool hasPos, bool hasNorms, bool hasUVs);
        static unsigned int createImported(std::string model_path);
        static unsigned int createPreset(MeshPreset type);
        static unsigned int createSkybox(std::string cubemap_dir);

        static void renderAll(glm::mat4 projection, glm::mat4 view, glm::vec3 camPos);
        static void renderModel(unsigned int modelID, glm::mat4 projection, glm::mat4 view, glm::vec3 camPos);
        static void renderPrimitive(unsigned int modelID, unsigned int meshID, glm::mat4 projection, glm::mat4 view, glm::vec3 camPos);
        static void deleteModel(unsigned int modelID);
        static Model* getModel(unsigned int modelID);
        static void setModelMaterialType(unsigned int modelID, unsigned int materialID, MaterialType type);
        
        // DEBUG
        static int getNumberOfModels();
        static void printPrimRelations(unsigned int modelID);
        static void renderPrim(unsigned int modelID, unsigned int meshID, glm::mat4 perspective, glm::mat4 view);

    private:
        static unsigned int nextModelID;
        static std::vector<ModelPrimitive*> opaquePrims;
        static std::vector<ModelPrimitive*> translucentPrims;
        static std::vector<ModelPrimitive*> cutoutPrims;
        static ModelPrimitive* skyboxPrim;

        // static void reorderByProgram();
        static void placeInCache(unsigned int modelID);
};