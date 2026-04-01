#pragma once

#include <vector>
#include <unordered_set>
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
    bool changeModelName(unsigned int modelID, std::string name);
    void changeMeshMaterial(unsigned int modelID, unsigned int meshIdx, unsigned int materialID);
    void changeModelMaterial(unsigned int modelID, unsigned int materialID);
    void deleteModel(unsigned int modelID);
    void setAsSkybox(unsigned int modelID);
    
    Model* getModel(unsigned int modelID);
    std::vector<Model*> getAllModels() const;
    unsigned int getSkyboxModelID() const;
    int getNumberOfModels();

    // FUNCTIONS THAT SHOULD NOT BE CALLED OUTSIDE OF RENDER PIPLINE/ ASSET CREATION
    bool reserveModelID(unsigned int ID, std::string model_path, ModelType type);
    unsigned int createModelForImportSetup(std::string model_path);
    void addPresetMesh(unsigned int ID, ModelType type);
    bool trySendingToRenderer(unsigned int modelID);

private:
    unsigned int skyboxModelID = INVALID_MODEL_ID; //initially invalid
    unsigned int nextModelID = 0;
    std::unordered_set<std::string> usedModelNames;
    std::unordered_map<unsigned int, std::unique_ptr<Model>> modelIDMap; 
    bool validateNextID();

    //SYSTEM FUNCTIONALITY
    bool initialized = false;
    Logger* loggerPtr                = nullptr;
    EventDispatcher* eventsPtr       = nullptr;
    PresetAssets* presetsPtr         = nullptr;
};
