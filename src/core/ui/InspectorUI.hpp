#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "object/Model.hpp"
#include "core/UniformTypes.hpp"
#include "core/FileRegistry.hpp"

struct ObjectShaderSelector {
    unsigned int modelID; //std::string objectName;
    int selection;
};

struct ObjectTextureSelector {
    unsigned int modelID; //std::string objectName;
    std::string uniformName;
    int textureSelection;
    int unitSelection;
};

class Logger;
class InspectorEngine;
class TextureRegistry;
class ShaderRegistry;
class UniformRegistry;
class EventDispatcher;
class ModelCache;

class InspectorUI {
public:
    InspectorUI();
    bool initialize(Logger* _loggerPtr, InspectorEngine* _inspectorEngPtr, TextureRegistry* _textureRegPtr, ShaderRegistry* _shaderRegPtr, UniformRegistry* _uniformRegPtr, EventDispatcher* _eventsPtr, ModelCache* _modelCachePtr, FileRegistry* _fileRegPtr);
    void shutdown();
    void render();
  
private:
    bool intitialized = false;
    Logger* loggerPtr = nullptr;
    InspectorEngine* inspectorEngPtr = nullptr;
    TextureRegistry* textureRegPtr = nullptr;
    ShaderRegistry* shaderRegPtr = nullptr;
    UniformRegistry* uniformRegPtr = nullptr;
    EventDispatcher* eventsPtr = nullptr;
    ModelCache* modelCachePtr = nullptr;
    FileRegistry* fileRegPtr = nullptr;
    std::vector<std::string> uniformNamesToDelete;
    std::string newUniformName;
    std::string newUniformShaderName;
    UniformType newUniformType = UniformType::NoType;
    std::unordered_map<unsigned int, ObjectShaderSelector> modelShaderSelectors;
    std::unordered_map<unsigned int, ObjectTextureSelector> modelTextureSelectors;

    void drawUniformInspector();
    void drawObjectsInspector();
    void drawAssetsInspector();
    void drawRenameFileEntry(ShaderFile* fileData);
    void drawDeleteFileEntity(ShaderFile* fileData);
    void drawStandardFileEntry(ShaderFile* fileData);
    void drawShaderFileInspector();
    void drawAddObjectMenu();
    bool drawShaderProgramSelector(ObjectShaderSelector& selector);
    bool drawTextureSelector(ObjectTextureSelector& selector);
    bool drawTextInput(std::string *value, const char *label);
    bool drawUniformInputValue(int* value);
    bool drawUniformInputValue(float* value);
    bool drawUniformInputValue(glm::vec3* value);
    bool drawUniformInputValue(glm::vec4* value);
    bool drawUniformInputValue(glm::mat4* value);
    bool drawUniformInputValue(glm::quat* value);
    void drawUniformInput(Uniform& uniform, unsigned int modelID);
    bool drawModelPositionInput(Model* model);
    bool drawModelScaleInput(Model* model);
    bool drawModelOrientationInput(Model* model);
};
