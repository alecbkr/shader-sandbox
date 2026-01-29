#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <unordered_map>
#include <vector>
#include "core/UniformTypes.hpp"

struct ObjectShaderMenu {
    std::string objectName;
    int selection;
    bool initialized;
};

struct ObjectTextureMenu {
    std::string objectName;
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
    static std::unordered_map<std::string, ObjectShaderMenu> objectShaderMenus;
    static std::unordered_map<std::string, ObjectTextureMenu> objectTextureMenus;
    static std::unordered_map<std::string, ShaderLinkMenu> shaderLinkMenus;

    static void drawUniformInspector();
    static void drawObjectsInspector();
    static void drawAssetsInspector();
    static void drawShaderFileInspector();
    static void drawAddObjectMenu();
    static void drawShaderLinkMenus(std::unordered_map<std::string, ShaderLinkMenu>& menus);
    static void drawShaderLinkMenu(ShaderLinkMenu& menu, const std::vector<const char*>& vertChoices, const std::vector<const char*>& geoChoices, const std::vector<const char*>& fragChoices);
    static void initializeMenu(ShaderLinkMenu& menu, const std::vector<const char*>& vertChoices, const std::vector<const char*>& geoChoices, const std::vector<const char*>& fragChoices);
    static void initializeMenu(ObjectShaderMenu& menu, const std::vector<const char*>& shaderChoices);
    static void initializeMenu(ObjectTextureMenu& menu);
    static bool drawShaderProgramMenu(ObjectShaderMenu& selector, const std::vector<const char*>& shaderChoices);
    static bool drawTextureMenu(ObjectTextureMenu& selector);
    static bool drawTextInput(std::string *value, const char *label);
    static bool drawUniformInputValue(int* value, Uniform* uniform = nullptr);
    static bool drawUniformInputValue(float* value, Uniform* uniform = nullptr);
    static bool drawUniformInputValue(glm::vec3* value, Uniform* uniform = nullptr);
    static bool drawUniformInputValue(glm::vec4* value, Uniform* uniform = nullptr);
    static bool drawUniformInputValue(glm::mat4* value, Uniform* uniform = nullptr);
    static bool drawUniformInputValue(glm::quat* value, Uniform* uniform = nullptr);
    static bool drawUniformInputValue(InspectorSampler2D* value, Uniform* uniform = nullptr);
    static void drawUniformInput(Uniform& uniform, const std::string& objectName);
};
