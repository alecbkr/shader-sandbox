#pragma once

#include <vector>
#include "CustomModel.hpp"
#include "ImportedModel.hpp"
#include "core/logging/Logger.hpp"
#include "core/InspectorEngine.hpp"
#include "core/EventDispatcher.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/UniformRegistry.hpp"

static constexpr unsigned int INVALID_MODEL = UINT_MAX;

class ModelCache {
public:
    ModelCache();
    bool initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, ShaderRegistry* _shaderRegPtr, UniformRegistry* _uniformRegPtr);
    void shutdown();
    unsigned int createModel(std::vector<float>, std::vector<unsigned int>, 
                                                    bool hasPos, bool hasNorms, bool hasUVs);
    unsigned int createModel(std::string pathname);
    
    void setTexture(unsigned int ID, std::string pathname, std::string uniformName);
    void setProgram(unsigned int ID, ShaderProgram& program);

    void translateModel(unsigned int ID, glm::vec3 pos);
    void scaleModel(unsigned int ID, glm::vec3 scale);
    void rotateModel(unsigned int ID, float angle, glm::vec3 axis);
    
    void renderModel(unsigned int ID, glm::mat4 projection, glm::mat4 view);
    void renderAll(glm::mat4 projection, glm::mat4 view);

    int getNumberOfModels();
    
    std::unordered_map<unsigned int, std::unique_ptr<Model>> modelIDMap; 
    
    Model* getModel(unsigned int ID);

    // DEBUG
    void printOrder();

    void setInspectorEnginePtr(InspectorEngine* _inspectorEngPtr);

private:
    bool initialized = false;
    bool inspectorEngPtrSet = false;
    Logger* loggerPtr = nullptr;
    InspectorEngine* inspectorEngPtr = nullptr;
    EventDispatcher* eventsPtr = nullptr;
    ShaderRegistry* shaderRegPtr = nullptr;
    UniformRegistry* uniformRegPtr = nullptr;
    unsigned int nextModelID = 0;
    void reorderByProgram();
    std::vector<std::unique_ptr<Model>> modelCache; 
    
};