#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "core/InspectorEngine.hpp"
#include "core/ShaderRegistry.hpp"
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
    static void render();
  
    private:
    static int height;
    static int width;
    static std::vector<std::string> uniformNamesToDelete;
    static std::string newUniformName;
    static std::string newUniformShaderName;
    static UniformType newUniformType;
    static std::unordered_map<std::string, ObjectShaderSelector> objectShaderSelectors;
    static std::unordered_map<std::string, ObjectTextureSelector> objectTextureSelectors;

    static void drawUniformInspector();
    static void drawObjectsInspector();
    static void drawAssetsInspector();
    static void drawShaderFileInspector();
    static void drawAddObjectMenu();
    static bool drawShaderProgramSelector(ObjectShaderSelector& selector);
    static bool drawTextureSelector(ObjectTextureSelector& selector);
    static bool drawTextInput(std::string *value, const char *label);
    static bool drawUniformInputValue(int* value);
    static bool drawUniformInputValue(float* value);
    static bool drawUniformInputValue(glm::vec3* value);
    static bool drawUniformInputValue(glm::vec4* value);
    static bool drawUniformInputValue(glm::mat4* value);
    static bool drawUniformInputValue(glm::quat* value);
    static void drawUniformInput(Uniform& uniform, const std::string& objectName);
};
