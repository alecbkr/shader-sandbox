#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "core/InspectorEngine.hpp"
#include "core/ShaderHandler.hpp"
#include "core/UniformRegistry.hpp"
#include "core/UniformTypes.hpp"

class InspectorUI {
    public:
    InspectorUI(InspectorEngine& eng, UniformRegistry& registry, ShaderHandler& handler);
    void render();
  
    private:
    InspectorEngine& engine;
    UniformRegistry& uniformRegistry;
    ShaderHandler& shaderHandler;
    int height;
    int width;
    std::vector<std::string> uniformNamesToDelete;
    std::string newUniformName;
    std::string newUniformShaderName;
    UniformType newUniformType = UniformType::NoType;

    void drawUniformEditors();
    void drawAddUniformMenu();
    void drawTextInput(std::string *value, const char *label);
    bool drawUniformInputValue(int* value);
    bool drawUniformInputValue(float* value);
    bool drawUniformInputValue(glm::vec3* value);
    bool drawUniformInputValue(glm::vec4* value);
    void drawUniformInput(const Uniform& uniform);
};
