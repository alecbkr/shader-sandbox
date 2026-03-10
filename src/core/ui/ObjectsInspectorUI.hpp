#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <string>
#include <unordered_map>
#include <vector>

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
class Logger;
class InspectorEngine;
class ShaderRegistry;
class TextureRegistry;
class ModelCache;
class Model;

class ObjectsInspectorUI {
public:
    void draw(Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, TextureRegistry* textureRegPtr, ModelCache* modelCachePtr);

    struct ObjectsInspectorThemeSettings {
        ImVec4 bgColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        ImVec4 bgColorHovered = ImVec4(bgColor.x * 1.5, bgColor.y * 1.5, bgColor.z * 1.5, 1.0f);
        float indentSize = 10;
    };

private:
    std::unordered_map<unsigned int, ModelShaderMenu> modelShaderMenus;
    std::unordered_map<unsigned int, ModelTextureMenu> modelTextureMenus;

    void drawAddObjectMenu(Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, ModelCache* modelCachePtr);
    void initializeMenu(ModelShaderMenu& menu, const std::vector<const char*>& shaderChoices, Logger* loggerPtr, ShaderRegistry* shaderRegPtr, ModelCache* modelCachePtr);
    void initializeMenu(ModelTextureMenu& menu, Logger* loggerPtr);
    bool drawShaderProgramMenu(ModelShaderMenu& menu, const std::vector<const char*>& shaderChoices, ShaderRegistry* shaderRegPtr, ModelCache* modelCachePtr, InspectorEngine* inspectorEngPtr);
    bool drawTextureMenu(ModelTextureMenu& menu, Logger* loggerPtr, TextureRegistry* textureRegPtr);
    bool drawTextInput(std::string* value, const char* label);
    bool drawModelPositionInput(Model* model);
    bool drawModelScaleInput(Model* model);
    bool drawModelOrientationInput(Model* model);
    ObjectsInspectorThemeSettings theme;

};
