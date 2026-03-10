#pragma once

#include "core/UniformTypes.hpp"
#include "object/MaterialCache.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <string>

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
        float kInputWidth = 72.0f;
        float kColorPickerSize = 100.0f;
    };

private:
    Logger* loggerPtr_ = nullptr;
    UniformRegistry* uniformRegPtr_ = nullptr;
    ModelCache* modelCachePtr_ = nullptr;
    MaterialCache* materialCachePtr_ = nullptr;

    bool drawTextInput(std::string* value, const char* label);
    bool drawUniformInputValue(int* value, Uniform* uniform = nullptr);
    bool drawUniformInputValue(float* value, Uniform* uniform = nullptr);
    bool drawUniformInputValue(glm::vec3* value, Uniform* uniform = nullptr);
    bool drawUniformInputValue(glm::vec4* value, Uniform* uniform = nullptr);
    bool drawUniformInputValue(glm::mat4* value, Uniform* uniform = nullptr);
    bool drawUniformInputValue(glm::quat* value, Uniform* uniform = nullptr);
    bool drawUniformInputValue(InspectorSampler2D* value, Uniform* uniform = nullptr);
    bool drawUniformInputValue(InspectorReference* value, Uniform* uniform = nullptr);
    void drawUniformInput(Uniform& uniform, unsigned int modelID, InspectorEngine* inspectorEngPtr);
    
    UniformInspectorThemeSettings theme;
};
