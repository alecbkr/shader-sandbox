#include "core/ui/InspectorUI.hpp"
#include "core/InspectorEngine.hpp"
#include "core/TextureRegistry.hpp"
#include "core/UniformRegistry.hpp"
#include "core/UniformTypes.hpp"
#include "engine/Errorlog.hpp"
#include "engine/ShaderProgram.hpp"
#include "object/ObjCache.hpp"
#include "object/Texture.hpp"
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

InspectorUI::InspectorUI() {}

void InspectorUI::render() {
    ImGui::Text("Inspector");
    if (ImGui::BeginTabBar("Inspector tabs")) {

        if (ImGui::BeginTabItem("Uniforms")) {
            drawUniformInspector();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Objects")) {
            drawObjectsInspector();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Assets")) {
            drawAssetsInspector();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Shader Files")) {
            drawShaderFileInspector(); // <- for lukas
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void InspectorUI::drawUniformInspector() {
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

void InspectorUI::drawObjectsInspector() {
    // TODO: an arbitrary high number, just somewhere above the uniform inspector's ids.
    int imGuiID = 100000;
    drawAddObjectMenu();
    for (auto &[objectName, object] : ObjCache::objMap) {
        if (!objectShaderSelectors.contains(objectName)) {
            objectShaderSelectors[objectName] = ObjectShaderSelector{ 
                .objectName = objectName,
                .selection = 0,
            };    
        }
        if (!objectTextureSelectors.contains(objectName)) {
            objectTextureSelectors[objectName] = ObjectTextureSelector{ 
                .objectName = objectName,
                .uniformName = "baseTex",
                .textureSelection = 0,
            };    
        }
        ObjectShaderSelector& shaderSelector = objectShaderSelectors[objectName];
        ObjectTextureSelector& textureSelector = objectTextureSelectors[objectName];

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
            if (ImGui::TreeNode("mesh")) {

                ImGui::TreePop();
            }
            if (ImGui::TreeNode("textures")) {
                drawTextureSelector(textureSelector);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("shader program")) {
                drawShaderProgramSelector(shaderSelector);
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }
}

void InspectorUI::drawAddObjectMenu() {
}

void InspectorUI::drawAssetsInspector() {
    for (const Texture* texPtr : TEXTURE_REGISTRY.readTextures()) {
        ImGui::Text("%s\n", texPtr->path.c_str());
    }
}


void InspectorUI::drawShaderFileInspector() {
    // Lucas can fill this in
}

/*
// this is from an old version
// keeping the code cause it's a nice template. get rid of it once we finish the inspector MVP or if we have the code elsewhere.
void InspectorUI::drawAddUniformMenu() {
    ImGui::Text("Add Uniform");
    drawTextInput(&newUniformName, "Uniform Name");

    // Build a list of shader names
    std::vector<const char *> shaderChoices;
    shaderChoices.reserve(ShaderRegistry::getNumberOfPrograms() + 1);
    shaderChoices.push_back("");

    auto& shaders = ShaderRegistry::getPrograms();
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
*/

bool InspectorUI::drawTextInput(std::string *value, const char *label) {
    bool changed = false;
    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), "%s", value->c_str());
    if (ImGui::InputText(label, buffer, sizeof(buffer))) {
        *value = buffer;
        changed = true;
    }
    return changed;
}

bool InspectorUI::drawShaderProgramSelector(ObjectShaderSelector& selector) {
    bool changed = false;
    std::vector<const char *> shaderChoices;
    shaderChoices.reserve(ShaderRegistry::getNumberOfPrograms());

    auto& shaders = ShaderRegistry::getPrograms();
    for (auto &[name, shader] : shaders) {
        shaderChoices.push_back(name.c_str());
    }
    // display combo box
    if (ImGui::Combo("Shader", &selector.selection, shaderChoices.data(),
                        (int)shaderChoices.size())) {
        changed = true;
    }

    if (!changed) return false;
    
    // add check in case we get more types
    ShaderProgram& selectedShader = *ShaderRegistry::getProgram(shaderChoices[selector.selection]);
    ObjCache::setProgram(selector.objectName, selectedShader); 
    return true;
}

bool InspectorUI::drawTextureSelector(ObjectTextureSelector& selector) {
    bool changed = false;
    std::vector<const char *> textureChoices;
    std::vector<Texture*> textures = TEXTURE_REGISTRY.readTextures();
    textureChoices.reserve(textures.size());
    for (Texture* tex : textures) {
        textureChoices.push_back(tex->path.c_str());
    }
    if (ImGui::Combo("Texture", &selector.textureSelection, textureChoices.data(),
                        (int)textureChoices.size())) {
        changed = true;
        std::cout << "changed texture" << std::endl;
    }
    if (ImGui::InputInt("Unit", &selector.unitSelection)) {
        changed = true;
        std::cout << "changed unit" << std::endl;
    }
    if (drawTextInput(&selector.uniformName, "Uniform Name")) {
        changed = true;
        std::cout << "changed uniform name" << std::endl;
    }

    if (!changed) return false;
    
    // add check in case we get more types
    Texture* selectedTexture = textures.at(selector.textureSelection);
    ObjCache::setTexture(selector.objectName, *selectedTexture, selector.unitSelection, selector.uniformName); 

    return true;
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
