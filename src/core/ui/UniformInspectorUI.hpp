#pragma once

#include "core/UniformTypes.hpp"
#include "object/MaterialCache.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

class Logger;
class InspectorEngine;
class ShaderRegistry;
class UniformRegistry;
class ModelCache;
struct Uniform;
class ShaderProgram;

class UniformInspectorUI {
public:
    void draw(Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, UniformRegistry* uniformRegPtr, ModelCache* modelCachePtr, MaterialCache* materialCachePtr);

    struct UniformInspectorThemeSettings {
        float inputWidth = 50.0f;
        float colorPickerWidthVec3 = 100.0f;
        float colorPickerWidthVec4 = 128.0f; // about 1.28x the width of vec3 color picker, otherwise it looks weird.
        ImVec4 bgColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        ImVec4 bgColorHovered = ImVec4(bgColor.x * 1.5f, bgColor.y * 1.5f, bgColor.z * 1.5f, 1.0f);
        float indentSize = 2.0f;
    };

private:
    Logger* loggerPtr_ = nullptr;
    UniformRegistry* uniformRegPtr_ = nullptr;
    ModelCache* modelCachePtr_ = nullptr;
    MaterialCache* materialCachePtr_ = nullptr;
    InspectorEngine* inspectorEngPtr_ = nullptr;
    
    void drawModelContainer(int& imGuiID, unsigned int modelID, const std::vector<unsigned int>& materialIDs);
    void drawMaterialContainer(unsigned int modelID, const std::vector<unsigned int>& materialIDs, int& imGuiID);
    bool drawInput(int* value, Uniform* uniform = nullptr);
    bool drawInput(float* value, Uniform* uniform = nullptr);
    bool drawInput(glm::vec3* value, Uniform* uniform = nullptr);
    bool drawInput(glm::vec4* value, Uniform* uniform = nullptr);
    bool drawInput(glm::mat4* value, Uniform* uniform = nullptr);
    bool drawInput(glm::quat* value, Uniform* uniform = nullptr);
    bool drawInput(InspectorSampler2D* value, Uniform* uniform = nullptr);
    bool drawInput(InspectorReference* value, Uniform* uniform = nullptr);
    void drawUniformRow(Uniform& uniform, unsigned int matID);
    bool isSimpleType(UniformType type);
    
    UniformInspectorThemeSettings theme;
};
