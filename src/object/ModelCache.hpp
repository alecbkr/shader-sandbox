#pragma once

#include <vector>

#include "Model.hpp"
#include "presets/PresetAssets.hpp"

class Logger;
class EventDispatcher;
class PresetAssets;

static constexpr unsigned int INVALID_MODEL_ID = UINT_MAX;


class ModelCache {
public:
    ModelCache();
    bool initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr,  PresetAssets* _presetsPtr);
    void shutdown();
    
    unsigned int createPreset(ModelType type);
    void changeMeshMaterial(unsigned int modelID, unsigned int meshIdx, unsigned int materialID);
    void changeModelMaterial(unsigned int modelID, unsigned int materialID);
    void deleteModel(unsigned int modelID);
    
    Model* getSkybox();
    Model* getModel(unsigned int modelID);
    std::vector<Model*> getAllModels() const;
    int getNumberOfModels();

    unsigned int createSkybox(std::string cubemap_dir);

    // FUNCTIONS THAT SHOULD NOT BE ACCESSED
    void addPresetMesh(unsigned int ID, ModelType type);
    bool reserveModelID(unsigned int ID, std::string model_path, ModelType type);
    unsigned int createModelForImportSetup(std::string model_path);
    void trySendingToRenderer(unsigned int modelID);

private:
    unsigned int nextModelID = 0;
    std::unordered_map<unsigned int, std::unique_ptr<Model>> modelIDMap; 
    Model* skyboxModel;
    bool validateNextID();

    //SYSTEM FUNCTIONALITY
    bool initialized = false;
    Logger* loggerPtr                = nullptr;
    EventDispatcher* eventsPtr       = nullptr;
    PresetAssets* presetsPtr         = nullptr;
};
