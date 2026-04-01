#pragma once

#include "core/UniformTypes.hpp"
#include "object/MaterialCache.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <unordered_map>

class Logger;
class InspectorEngine;
class ShaderRegistry;
class UniformRegistry;
class ModelCache;
struct Uniform;
class ShaderProgram;
class Fonts;
struct SettingsStyles;

class UniformInspectorUI {
public:
    UniformInspectorUI() = default;
    UniformInspectorUI(Fonts* fonts, SettingsStyles* styles);
    ~UniformInspectorUI();

    void draw(Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, UniformRegistry* uniformRegPtr, ModelCache* modelCachePtr, MaterialCache* materialCachePtr);

    struct UniformInspectorThemeSettings {
        float inputWidth = 50.0f;
        float colorPickerWidthVec3 = 100.0f;
        float colorPickerWidthVec4 = 128.0f; // about 1.28x the width of vec3 color picker, otherwise it looks weird.
        ImVec4 cardBGColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        ImVec4 headerColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        ImVec4 headerColorHovered = ImVec4(headerColor.x * 1.5f, headerColor.y * 1.5f, headerColor.z * 1.5f, 1.0f);
        float indentSize = 5.0f;
    };

private:
    Logger* loggerPtr_ = nullptr;
    UniformRegistry* uniformRegPtr_ = nullptr;
    ModelCache* modelCachePtr_ = nullptr;
    MaterialCache* materialCachePtr_ = nullptr;
    InspectorEngine* inspectorEngPtr_ = nullptr;
    Fonts* fonts_ = nullptr;
    SettingsStyles* styles_ = nullptr;
    
    void drawModelContainer(int& imGuiID, unsigned int modelID, const std::unordered_map<unsigned int, unsigned int>& materialRefernces);
    void drawMaterialContainer(unsigned int modelID, const std::unordered_map<unsigned int, unsigned int>& materialReferences, int& imGuiID);
    bool drawInput(int* value, Uniform* uniform = nullptr);
    bool drawInput(float* value, Uniform* uniform = nullptr);
    bool drawInput(glm::vec3* value, Uniform* uniform = nullptr);
    bool drawInput(glm::vec4* value, Uniform* uniform = nullptr);
    bool drawInput(glm::mat4* value, Uniform* uniform = nullptr);
    bool drawInput(glm::quat* value, Uniform* uniform = nullptr);
    bool drawInput(InspectorSampler2D* value, Uniform* uniform = nullptr);
    bool drawInput(InspectorReference* value, Uniform* uniform = nullptr);
    bool drawReferenceEditor(InspectorReference* value, Uniform* uniform);
    void drawUniformRow(Uniform& uniform, unsigned int matID);
    bool drawModePicker(const char* id, int& mode, const char* const* labels, int labelCount);
    void setReferenceMode(Uniform& uniform, bool useReference);
    std::string makeUniformStateKey(unsigned int matID, const std::string& uniformName) const;
    std::string getUniformSummary(const Uniform& uniform) const;
    std::string getReferenceSummary(const Uniform& uniform) const;
    bool isSimpleType(UniformType type);
    void drawUniformsNested_byCursor(const std::unordered_map<std::string, Uniform>& uniforms, unsigned int matID, int& imGuiID);
    bool drawCompactTreeNode(const std::string& label);
    
    UniformInspectorThemeSettings theme;
};
