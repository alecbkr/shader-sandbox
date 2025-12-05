#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "core/InspectorEngine.hpp"
#include "core/UniformTypes.hpp"

class InspectorUI {
  public:
    int height;
    int width;
    InspectorEngine engine;
    
    InspectorUI() {

    }

    void render() {
        ImGui::Text("Object Properties");
        drawUniformEditors();
    }

  private:
    std::vector<std::string> uniformNamesToDelete;
    std::string newUniformName;
    std::string newUniformShaderName;
    UniformType newUniformType = UniformType::NoType;

    void drawUniformEditors() {
        drawAddUniformMenu();
        int imGuiID = 0;
        for (auto &[shaderName, shaderMap] : engine.uniforms) {
            ImGui::Text("%s", ("Shader:" + shaderName).c_str());
            for (auto &[uniformName, uniform] : engine.uniforms.at(shaderName)) {
                ImGui::PushID(imGuiID);
                drawUniformInput(uniform);
                ImGui::PopID();
                imGuiID++;
            }
            for (std::string s : uniformNamesToDelete)
                engine.uniforms.at(shaderName).erase(s);
            uniformNamesToDelete.clear();
        }
    }

    void drawAddUniformMenu() {
        ImGui::Text("Add Uniform");
        drawTextInput(&newUniformName, "Uniform Name");

        // Build a list of shader names
        std::vector<const char *> shaderChoices;
        shaderChoices.reserve(ShaderHandler::getNumberOfPrograms() + 1);
        shaderChoices.push_back("");

        auto& shaders = ShaderHandler::getPrograms();
        for (auto &[name, shader] : shaders) {
            shaderChoices.push_back(name.c_str());
        }
        // Keep track of the selected shader index
        static int shaderChoice = 0;
        if (newUniformShaderName == "")
            shaderChoice = 0;
        // Display combo box
        if (ImGui::Combo("Shader", &shaderChoice, shaderChoices.data(),
                         (int)shaderChoices.size())) {
            // When selection changes, update newUniformShaderName
            newUniformShaderName = shaderChoices[shaderChoice];
        }
        // Dropdown menu from enum
        const char *typeNames[6] = {"None", "Int",  "Float",
                                    "Vec3", "Vec4", "Reference"};
        int choice = static_cast<int>(newUniformType);
        if (ImGui::Combo("Uniform Type", &choice, typeNames,
                         IM_ARRAYSIZE(typeNames))) {
            newUniformType = static_cast<UniformType>(choice);
        }

        if (ImGui::Button("Add Uniform", ImVec2(100, 20))) {
            bool uniqueValidUniform = false;
            bool validShaderName = shaders.count(newUniformShaderName) >= 1;
            if (validShaderName) {
                uniqueValidUniform =
                    engine.uniforms[newUniformShaderName].count(newUniformName) <= 0 &&
                    newUniformType != UniformType::NoType;
            }

            if (uniqueValidUniform) {
                Uniform newUniform;
                newUniform.programName = newUniformShaderName;
                newUniform.name = newUniformName;
                newUniform.type = newUniformType;

                switch (newUniformType) {
                case UniformType::Int:
                    newUniform.value = 0;
                    break;
                case UniformType::Float:
                    newUniform.value = 0.0f;
                    break;
                case UniformType::Vec3:
                    newUniform.value = glm::vec3(0.0f);
                    break;
                case UniformType::Vec4:
                    newUniform.value = glm::vec4(0.0f);
                    break;
                default:
                    std::cout << "invalid new uniform type, making it an int"
                              << std::endl;
                    newUniform.type = UniformType::Int;
                    newUniform.value = 0;
                    break;
                }

                engine.uniforms[newUniformShaderName][newUniformName] = newUniform;
                newUniformShaderName = "";
                newUniformName = "";
            } else {
                std::cout << "bad new uniform input" << std::endl;
            }
        }
    }

    void drawTextInput(std::string *value, const char *label) {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "%s", value->c_str());
        if (ImGui::InputText(label, buffer, sizeof(buffer))) {
            *value = buffer;
        }
    }

    bool drawUniformInputValue(int* value) {
        return ImGui::InputInt("value", value);
    }

    bool drawUniformInputValue(float* value) {
        return ImGui::InputFloat("value", value);
    }

    bool drawUniformInputValue(glm::vec3* value) {
        bool changed = false;
        changed |= ImGui::InputFloat("x", &value->x);
        changed |= ImGui::InputFloat("y", &value->y);
        changed |= ImGui::InputFloat("z", &value->z);
        return changed;
    }

    bool drawUniformInputValue(glm::vec4* value) {
        bool changed = false;
        changed |= ImGui::InputFloat("x", &value->x);
        changed |= ImGui::InputFloat("y", &value->y);
        changed |= ImGui::InputFloat("z", &value->z);
        changed |= ImGui::InputFloat("w", &value->w);
        return changed;
    }

    void drawUniformInput(Uniform& uniform) {
        ImGui::Text("%s", uniform.name.c_str());
        
        bool changed = false;
        std::visit([&](auto& val){
            changed = drawUniformInputValue(&val);
        }, uniform.value);

        if (changed) engine.applyUniform(uniform.programName, uniform);

        if (ImGui::Button("Delete Uniform", ImVec2(100, 20))) {
            uniformNamesToDelete.push_back(uniform.name);
        }
    }
};