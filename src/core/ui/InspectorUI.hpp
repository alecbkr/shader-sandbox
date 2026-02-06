#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "object/Model.hpp"
#include "core/UniformTypes.hpp"
#include "core/FileRegistry.hpp"

struct ModelShaderMenu {
    unsigned int modelID; //std::string objectName;
    int selection;
    bool initialized;
};

struct ModelTextureMenu {
    unsigned int modelID; //std::string objectName;
    std::string uniformName;
    int textureSelection;
    int unitSelection;
    bool initialized;
};

struct ShaderLinkMenu {
    std::string shaderName;
    int vertSelection;
    int geometrySelection;
    int fragSelection;
    bool initialized;
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
    int height = 0;
    int width = 0;
    std::string newUniformName;
    std::string newUniformShaderName;
    UniformType newUniformType = UniformType::NoType;
    std::unordered_map<unsigned int, ModelShaderMenu> modelShaderMenus;
    std::unordered_map<unsigned int, ModelTextureMenu> modelTextureMenus;
    std::unordered_map<std::string, ShaderLinkMenu> shaderLinkMenus;

    void drawUniformInspector();
    void drawObjectsInspector();
    void drawAssetsInspector();
    void drawRenameFileEntry(ShaderFile* fileData);
    void drawDeleteFileEntity(ShaderFile* fileData);
    void drawStandardFileEntry(ShaderFile* fileData);
    void drawShaderFileInspector();
    void drawAddObjectMenu();
    void drawShaderLinkMenus(std::unordered_map<std::string, ShaderLinkMenu>& menus);
    void drawShaderLinkMenu(ShaderLinkMenu& menu, const std::vector<const char*>& vertChoices, const std::vector<const char*>& geoChoices, const std::vector<const char*>& fragChoices);
    void initializeMenu(ShaderLinkMenu& menu, const std::vector<const char*>& vertChoices, const std::vector<const char*>& geoChoices, const std::vector<const char*>& fragChoices);
    void initializeMenu(ModelShaderMenu& menu, const std::vector<const char*>& shaderChoices);
    void initializeMenu(ModelTextureMenu& menu);
    bool drawShaderProgramMenu(ModelShaderMenu& selector, const std::vector<const char*>& shaderChoices);
    bool drawTextureMenu(ModelTextureMenu& selector);
    bool drawTextInput(std::string *value, const char *label);
    bool drawUniformInputValue(int* value, Uniform* uniform = nullptr);
    bool drawUniformInputValue(float* value, Uniform* uniform = nullptr);
    bool drawUniformInputValue(glm::vec3* value, Uniform* uniform = nullptr);
    bool drawUniformInputValue(glm::vec4* value, Uniform* uniform = nullptr);
    bool drawUniformInputValue(glm::mat4* value, Uniform* uniform = nullptr);
    bool drawUniformInputValue(glm::quat* value, Uniform* uniform = nullptr);
    bool drawUniformInputValue(InspectorSampler2D* value, Uniform* uniform = nullptr);
    void drawUniformInput(Uniform& uniform, const std::string& objectName);
    void drawUniformInput(Uniform& uniform, unsigned int modelID);
    bool drawModelPositionInput(Model* model);
    bool drawModelScaleInput(Model* model);
    bool drawModelOrientationInput(Model* model);
};
