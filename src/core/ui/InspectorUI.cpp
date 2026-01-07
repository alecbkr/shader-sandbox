#include "core/ui/InspectorUI.hpp"
#include "core/InspectorEngine.hpp"
#include "core/ShaderHandler.hpp"
#include "core/UniformRegistry.hpp"
#include "core/UniformTypes.hpp"
#include "engine/Errorlog.hpp"
#include "object/ObjCache.hpp"
#include <string>
#include <unordered_map>

InspectorUI::InspectorUI() {}

void InspectorUI::render() {
    ImGui::Text("Inspector");
    if (ImGui::BeginTabBar("Inspector tabs")) {

        if (ImGui::BeginTabItem("Uniforms")) {
            drawUniformInspector();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("World Data")) {
            drawWorldDataInspector();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void InspectorUI::drawWorldDataInspector() {
    // TODO: an arbitrary high number, just somewhere above the uniform inspector's ids.
    int imGuiID = 100000;
    for (auto &[objectName, object] : ObjCache::objMap) {
        if (ImGui::TreeNode(objectName.c_str())) {
            if (ImGui::TreeNode("position")) {
                drawUniformInputValue(&object->objPosition);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("scale")) {
                drawUniformInputValue(&object->objScale);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("orientation")) {
                drawUniformInputValue(&object->objOrientation);
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }
}

void InspectorUI::drawUniformInspector() {
    drawAddUniformMenu();
    int imGuiID = 0;
    for (auto &[objectName, object] : ObjCache::objMap) {
        if (ImGui::TreeNode(objectName.c_str())) {
            const std::unordered_map<std::string, Uniform>* uniformMap = UNIFORM_REGISTRY.tryReadUniforms(objectName);

            if (uniformMap == nullptr) {
                Errorlog::getInstance().logEntry(EL_WARNING, "drawUniformInspector", ("Object not found in registry: " + objectName).c_str());
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
                UNIFORM_REGISTRY.eraseUniform(objectName, uniformName);
            uniformNamesToDelete.clear();

            ImGui::TreePop();
        }
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
                UNIFORM_REGISTRY.containsUniform(newUniformShaderName, newUniformName) &&
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
            case UniformType::Mat4:
                newUniform.value = glm::mat4(0.0f);
                break;
            default:
                std::cout << "invalid new uniform type, making it an int"
                            << std::endl;
                newUniform.type = UniformType::Int;
                newUniform.value = 0;
                break;
            }

            UNIFORM_REGISTRY.registerUniform(newUniformShaderName, newUniform);
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

bool InspectorUI::drawUniformInputValue(glm::quat* value) {
    bool changed = false;
    changed |= ImGui::InputFloat("x", &value->x);
    changed |= ImGui::InputFloat("y", &value->y);
    changed |= ImGui::InputFloat("z", &value->z);
    changed |= ImGui::InputFloat("w", &value->w);
    return changed;
}

bool InspectorUI::drawUniformInputValue(glm::mat4* value) {
    static int tableID = 0;
    const int columns = 4;
    tableID++;
    bool changed = false;
    if (ImGui::BeginTable(("Matrix4x4" + std::to_string(tableID)).c_str(), columns, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        for (int row = 0; row < 4; ++row) {
            ImGui::TableNextRow();
            for (int col = 0; col < 4; ++col) {
                ImGui::TableSetColumnIndex(col);
                ImGui::PushID(row * 4 + col); // unique ID
                ImGui::PushItemWidth(60);
                ImGui::InputFloat("", &(*value)[col][row]); // column-major
                ImGui::PopItemWidth();
                ImGui::PopID();
            }
        }
        ImGui::EndTable();
    }    
    return changed;
}

void InspectorUI::drawUniformInput(Uniform& uniform, const std::string& objectName) {
    if (ImGui::TreeNode(uniform.name.c_str())) {
        bool changed = false;
        std::visit([&](auto& val){
            changed = drawUniformInputValue(&val);
        }, uniform.value);

        if (changed) {
            InspectorEngine::applyInput(objectName, uniform);
        }

        if (ImGui::Button("Delete Uniform", ImVec2(100, 20))) {
            uniformNamesToDelete.push_back(uniform.name);
        }
        ImGui::TreePop();
    }
}
