#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "core/InspectorEngine.hpp"
#include "core/ShaderHandler.hpp"
#include "core/UniformRegistry.hpp"
#include "object/Object.hpp"
#include "core/UniformTypes.hpp"

struct ObjectShaderSelector {
    std::string objectName;
    int selection;
};

struct ObjectTextureSelector {
    std::string objectName;
    std::string uniformName;
    int textureSelection;
    int unitSelection;
};

class InspectorUI {
    public:
    InspectorUI();
    void render();
  
    private:
    int height;
    int width;
    std::vector<std::string> uniformNamesToDelete;
    std::string newUniformName;
    std::string newUniformShaderName;
    UniformType newUniformType = UniformType::NoType;
    std::unordered_map<std::string, ObjectShaderSelector> objectShaderSelectors;
    std::unordered_map<std::string, ObjectTextureSelector> objectTextureSelectors;

    void drawUniformInspector();
    void drawObjectsInspector();
    void drawAssetsInspector();
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
    void drawUniformInput(Uniform& uniform, const std::string& objectName);
};
