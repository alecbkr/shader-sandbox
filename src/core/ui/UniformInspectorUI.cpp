#include "core/ui/UniformInspectorUI.hpp"

#include "core/InspectorEngine.hpp"
#include "core/logging/Logger.hpp"
#include "core/UniformRegistry.hpp"
#include "core/UniformTypes.hpp"
#include "core/ShaderRegistry.hpp"
#include "engine/ShaderProgram.hpp"
#include "object/ModelCache.hpp"
#include <string>
#include <unordered_map>
#include <vector>

void UniformInspectorUI::draw(Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, UniformRegistry* uniformRegPtr, ModelCache* modelCachePtr) {
    loggerPtr_ = loggerPtr;
    uniformRegPtr_ = uniformRegPtr;
    modelCachePtr_ = modelCachePtr;

    int imGuiID = 0;
    for (auto& [modelID, model] : modelCachePtr->modelIDMap) {
        ShaderProgram* modelProgram = shaderRegPtr->getProgram(model->getProgramID());
        if (modelProgram == nullptr) {
            continue;
        }
        std::string label = "model " + std::to_string(modelID);
        if (ImGui::TreeNode(label.c_str())) {
            const std::unordered_map<std::string, Uniform>* uniformMap = uniformRegPtr->tryReadUniforms(modelID);

            if (uniformMap == nullptr) {
                loggerPtr_->addLog(LogLevel::WARNING, "drawUniformInspector", "Object not found in registry: ", std::to_string(modelID));
                ImGui::TreePop();
                continue;
            }

            for (auto& [uniformName, uniformRef] : *uniformMap) {
                ImGui::PushID(imGuiID);
                Uniform uniformCopy = uniformRef;
                drawUniformInput(uniformCopy, modelID, inspectorEngPtr);
                ImGui::PopID();
                imGuiID++;
            }
            ImGui::TreePop();
        }
    }
}

bool UniformInspectorUI::drawTextInput(std::string* value, const char* label) {
    bool changed = false;
    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), "%s", value->c_str());
    if (ImGui::InputText(label, buffer, sizeof(buffer))) {
        *value = buffer;
        changed = true;
    }
    return changed;
}

bool UniformInspectorUI::drawUniformInputValue(int* value, Uniform* uniform) {
    return ImGui::InputInt("value", value);
}

bool UniformInspectorUI::drawUniformInputValue(float* value, Uniform* uniform) {
    return ImGui::InputFloat("value", value);
}

bool UniformInspectorUI::drawUniformInputValue(glm::vec3* value, Uniform* uniform) {
    bool changed = false;
    bool useColorPicker = false;
    if (uniform != nullptr) {
        changed |= ImGui::Checkbox("Use Color Picker", &uniform->useAlternateEditor);
        useColorPicker = uniform->useAlternateEditor;
    }

    if (useColorPicker) {
        changed |= ImGui::ColorPicker3("", &value->x);
    } else {
        changed |= ImGui::InputFloat("x", &value->x);
        changed |= ImGui::InputFloat("y", &value->y);
        changed |= ImGui::InputFloat("z", &value->z);
    }
    return changed;
}

bool UniformInspectorUI::drawUniformInputValue(glm::vec4* value, Uniform* uniform) {
    bool changed = false;
    bool useColorPicker = false;
    if (uniform != nullptr) {
        ImGui::Checkbox("Use Color Picker", &uniform->useAlternateEditor);
        useColorPicker = uniform->useAlternateEditor;
    }

    if (useColorPicker) {
        changed |= ImGui::ColorPicker4("", &value->x);
    } else {
        changed |= ImGui::InputFloat("x", &value->x);
        changed |= ImGui::InputFloat("y", &value->y);
        changed |= ImGui::InputFloat("z", &value->z);
        changed |= ImGui::InputFloat("w", &value->w);
    }
    return changed;
}

bool UniformInspectorUI::drawUniformInputValue(glm::quat* value, Uniform* uniform) {
    bool changed = false;
    changed |= ImGui::InputFloat("x", &value->x);
    changed |= ImGui::InputFloat("y", &value->y);
    changed |= ImGui::InputFloat("z", &value->z);
    changed |= ImGui::InputFloat("w", &value->w);
    return changed;
}

bool UniformInspectorUI::drawUniformInputValue(glm::mat4* value, Uniform* uniform) {
    static int tableID = 0;
    const int columns = 4;
    tableID++;
    bool changed = false;
    if (ImGui::BeginTable(("Matrix4x4" + std::to_string(tableID)).c_str(), columns, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        for (int row = 0; row < 4; ++row) {
            ImGui::TableNextRow();
            for (int col = 0; col < 4; ++col) {
                ImGui::TableSetColumnIndex(col);
                ImGui::PushID(row * 4 + col);
                ImGui::PushItemWidth(60);
                ImGui::InputFloat("", &(*value)[col][row]);
                ImGui::PopItemWidth();
                ImGui::PopID();
            }
        }
        ImGui::EndTable();
    }
    return changed;
}

bool UniformInspectorUI::drawUniformInputValue(InspectorSampler2D* value, Uniform* uniform) {
    return drawUniformInputValue(&value->textureUnit);
}

bool UniformInspectorUI::drawUniformInputValue(InspectorReference* value, Uniform* uniform) {
    value->initialized = false;
    value->useCamaraData = false;
    bool changed = false;
    std::vector<std::string> modelNames;
    std::vector<const char*> modelChoices{""};
    std::vector<unsigned int> modelIDs{0};
    modelNames.reserve(modelCachePtr_->modelIDMap.size());
    modelChoices.reserve(modelCachePtr_->modelIDMap.size() + 1);
    modelIDs.reserve(modelCachePtr_->modelIDMap.size() + 1);

    std::optional<std::vector<std::string>> worldData = getWorldData(uniform->type);

    if (worldData || value->useWorldData) {
        changed |= ImGui::Checkbox("Use World Data?", &value->useWorldData);
    }

    int i = 0;
    for (auto& [modelID, model] : modelCachePtr_->modelIDMap) {
        if (modelID == uniform->modelID) {
            continue;
        }
        modelNames.push_back(std::to_string(modelID));
        modelChoices.push_back(modelNames[i].c_str());
        modelIDs.push_back(modelID);
        i++;
    }

    if (value->useWorldData) {
        modelChoices.push_back("camera");
    }

    changed |= ImGui::Combo("Models", &value->modelSelection, modelChoices.data(), modelChoices.size());

    if (changed) {
        value->uniformSelection = 0;
    }
    if (value->modelSelection == 0) {
        return changed;
    }

    if (std::string(modelChoices[value->modelSelection]) == "camera") {
        value->useCamaraData = true;
    } else {
        value->referencedModelID = modelIDs[value->modelSelection];
    }

    std::vector<const char*> uniformChoices{""};

    if (!value->useWorldData) {
        const auto modelUniforms = uniformRegPtr_->tryReadUniforms(value->referencedModelID);
        if (modelUniforms == nullptr) {
            loggerPtr_->addLog(LogLevel::LOG_ERROR, "drawUniformInputValue", "modelID " + std::to_string(value->referencedModelID) + " not found in Uniform Registry!");
            return false;
        }

        for (auto& [uniformName, uniform] : *modelUniforms) {
            if (uniform.type != value->returnType) {
                continue;
            }
            uniformChoices.push_back(uniformName.c_str());
        }
    } else {
        if (!worldData) {
            loggerPtr_->addLog(LogLevel::LOG_ERROR, "drawUniformInputValue", "world data is not enabled for this type");
        }
        for (const std::string& worldDataStr : worldData.value()) {
            uniformChoices.push_back(worldDataStr.c_str());
        }
    }

    changed |= ImGui::Combo("Uniforms", &value->uniformSelection, uniformChoices.data(), uniformChoices.size());

    if (value->uniformSelection > 0) {
        value->referencedUniformName = uniformChoices[value->uniformSelection];
        value->initialized = true;
    }

    return changed;
}

void UniformInspectorUI::drawUniformInput(Uniform& uniform, unsigned int modelID, InspectorEngine* inspectorEngPtr) {
    if (ImGui::TreeNode(uniform.name.c_str())) {
        bool changed = false;
        bool changedFunctionBox = false;

        changedFunctionBox |= ImGui::Checkbox("Use Function", &uniform.isFunction);

        if (changedFunctionBox) {
            if (uniform.isFunction) {
                uniform.isFunction = true;
                uniform.value = InspectorReference{
                    .modelSelection = 0, .uniformSelection = 0,
                    .returnType = uniform.type,
                    .initialized = false,
                };
            } else {
                uniform.isFunction = false;
                uniform.value = inspectorEngPtr->getDefaultValue(uniform.type);
            }
        }

        std::visit([&](auto& val) {
            changed = drawUniformInputValue(&val, &uniform);
        }, uniform.value);

        if (changed || changedFunctionBox) {
            inspectorEngPtr->applyInput(modelID, uniform);
        }

        ImGui::TreePop();
    }
}
