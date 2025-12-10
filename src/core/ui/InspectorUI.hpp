#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "core/InspectorEngine.hpp"
#include "core/UniformTypes.hpp"

class InspectorUI {
  public:
  InspectorEngine engine;
  
  InspectorUI();
  void render();
  
  private:
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
    void drawUniformInput(Uniform& uniform);
};