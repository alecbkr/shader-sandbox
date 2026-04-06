#pragma once

#include "object/MaterialCache.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <string>
#include <unordered_map>
#include <vector>

class ModalManager; 

struct MaterialShaderMenu {
    unsigned int matID; //std::string objectName;
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
class Logger;
class InspectorEngine;
class ShaderRegistry;
class TextureRegistry;
class ModelCache;
class Model;
struct SettingsStyles;
class Fonts;

class ObjectsInspectorUI {
public:
    ObjectsInspectorUI() = default;
    explicit ObjectsInspectorUI(SettingsStyles* styles);

    void draw(Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, TextureRegistry* textureRegPtr, ModelCache* modelCachePtr, MaterialCache* materialCachePtr, Fonts* fonts, ModalManager* modalManager);

    struct ObjectsInspectorThemeSettings {
        ImVec4 bgColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        ImVec4 bgColorHovered = ImVec4(bgColor.x * 1.5, bgColor.y * 1.5, bgColor.z * 1.5, 1.0f);
        float indentSize = 10;
    };

private:
    std::unordered_map<unsigned int, MaterialShaderMenu> materialShaderMenus;
    std::unordered_map<unsigned int, ModelTextureMenu> modelTextureMenus;
    SettingsStyles* styles = nullptr;

    //void drawAddObjectMenu(Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, ModelCache* modelCachePtr);
    void initializeMenu(MaterialShaderMenu& menu, const std::vector<unsigned int>& shaderChoices, Logger* loggerPtr, ShaderRegistry* shaderRegPtr, MaterialCache* materialCachePtr);
    void initializeMenu(ModelTextureMenu& menu, Logger* loggerPtr);
    // bool drawShaderProgramMenu(MaterialShaderMenu& menu, const std::vector<const char*>& shaderChoices, ShaderRegistry* shaderRegPtr, MaterialCache* materialCachePtr, InspectorEngine* inspectorEngPtr, Logger* logger);
    bool drawMeshesMenu(Model* currModel, MaterialCache* materialCachePtr, ModelCache* modelCachePtr, Logger* loggerPtr);
    bool drawInstancesMenu(Model* currModel, ModelCache* modelCachePtr, Logger* loggerPtr);
    bool drawAdditionalMenu(Model* currModel, ModelCache* modelCachePtr, Logger* loggerPtr);
    bool drawTextureMenu(ModelTextureMenu& menu, Logger* loggerPtr, TextureRegistry* textureRegPtr);
    bool drawTextInput(std::string* value, const char* label);
    bool drawModelPositionInput(Model& model);
    bool drawModelScaleInput(Model& model);
    bool drawModelOrientationInput(Model& model);
    ObjectsInspectorThemeSettings theme;

};
