#include "core/ui/InspectorUI.hpp"
#include "core/ShaderHandler.hpp"
#include "core/UniformRegistry.hpp"
#include "engine/Errorlog.hpp"
#include "object/ObjCache.hpp"
#include <unordered_map>

InspectorUI::InspectorUI(InspectorEngine& eng): engine(eng), uniformRegistry(UniformRegistry::instance()) {}

void InspectorUI::render() {
    ImGui::Text("Object Properties");
    drawUniformEditors();
}

void InspectorUI::drawUniformEditors() {
    drawAddUniformMenu();
    int imGuiID = 0;
    for (auto &[objectName, object] : ObjCache::objMap) {
        ImGui::Text("%s", ("Object:" + objectName).c_str());
        const std::unordered_map<std::string, Uniform>* uniformMap = uniformRegistry.tryReadUniforms(objectName);

        if (uniformMap == nullptr) {
            Errorlog::getInstance().logEntry(EL_WARNING, "drawUniformEditors", ("Object not found in registry: " + objectName).c_str());
            continue;
        }

        for (auto &[uniformName, uniformRef] : *uniformMap) {
            ImGui::PushID(imGuiID);
            Uniform uniformCopy = uniformRef;
            drawUniformInput(uniformCopy, objectName);
            ImGui::PopID();
            imGuiID++;
        }
        for (std::string uniformName : uniformNamesToDelete)
            uniformRegistry.eraseUniform(objectName, uniformName);
        uniformNamesToDelete.clear();
    }
}

void InspectorUI::drawAddUniformMenu() {
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

    if (ImGui::Button("Add Uniform", ImVec2(100, 25))) {
        bool uniqueValidUniform = false;
        bool validShaderName = shaders.count(newUniformShaderName) >= 1;
        if (validShaderName) {
            uniqueValidUniform =
                uniformRegistry.containsUniform(newUniformShaderName, newUniformName) &&
                newUniformType != UniformType::NoType;
        }

        if (uniqueValidUniform) {
            Uniform newUniform;
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

            uniformRegistry.registerUniform(newUniformShaderName, newUniform);
            newUniformShaderName = "";
            newUniformName = "";
        } else {
            std::cout << "bad new uniform input" << std::endl;
        }
    }
}

void InspectorUI::drawTextInput(std::string *value, const char *label) {
    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), "%s", value->c_str());
    if (ImGui::InputText(label, buffer, sizeof(buffer))) {
        *value = buffer;
    }
}

bool InspectorUI::drawUniformInputValue(int* value) {
    return ImGui::InputInt("value", value);
}

bool InspectorUI::drawUniformInputValue(float* value) {
    return ImGui::InputFloat("value", value);
}

bool InspectorUI::drawUniformInputValue(glm::vec3* value) {
    bool changed = false;
    changed |= ImGui::InputFloat("x", &value->x);
    changed |= ImGui::InputFloat("y", &value->y);
    changed |= ImGui::InputFloat("z", &value->z);
    return changed;
}

bool InspectorUI::drawUniformInputValue(glm::vec4* value) {
    bool changed = false;
    changed |= ImGui::InputFloat("x", &value->x);
    changed |= ImGui::InputFloat("y", &value->y);
    changed |= ImGui::InputFloat("z", &value->z);
    changed |= ImGui::InputFloat("w", &value->w);
    return changed;
}

void InspectorUI::drawUniformInput(Uniform& uniform, const std::string& objectName) {
    ImGui::Text("%s", uniform.name.c_str());
    
    bool changed = false;
    std::visit([&](auto& val){
        changed = drawUniformInputValue(&val);
    }, uniform.value);

    if (changed) {
        engine.applyInput(objectName, uniform);
    }

    if (ImGui::Button("Delete Uniform", ImVec2(100, 20))) {
        uniformNamesToDelete.push_back(uniform.name);
    }
}
