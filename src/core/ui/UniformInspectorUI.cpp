#include "core/ui/UniformInspectorUI.hpp"

#include "core/InspectorEngine.hpp"
#include "core/logging/Logger.hpp"
#include "core/UniformRegistry.hpp"
#include "core/UniformTypes.hpp"
#include "core/ShaderRegistry.hpp"
#include "engine/ShaderProgram.hpp"
#include "object/MaterialCache.hpp"
#include "object/ModelCache.hpp"
#include <string>
#include <unordered_map>
#include <vector>

// ai did this, need to change this to just be a field of the class
namespace {
const float kInputWidth = 72.0f;
const float kColorPickerSize = 100.0f;
}

void UniformInspectorUI::draw(Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, UniformRegistry* uniformRegPtr, ModelCache* modelCachePtr, MaterialCache* materialCachePtr) {
    loggerPtr_ = loggerPtr;
    uniformRegPtr_ = uniformRegPtr;
    modelCachePtr_ = modelCachePtr;
    materialCachePtr_ = materialCachePtr;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 4.0f));

    int imGuiID = 0;
    for (auto& [modelID, model] : modelCachePtr->modelIDMap) {
        std::string modelLabel = "Model " + std::to_string(modelID) + "##uniform_model_" + std::to_string(modelID);
        if (ImGui::TreeNodeEx(modelLabel.c_str(),  ImGuiTreeNodeFlags_Framed)) {
            for (unsigned int matID : model->getAllMaterialIDs()) {
                std::string matLabel = "Material " + std::to_string(matID) + "##uniform_mat_" + std::to_string(matID);
                if (ImGui::TreeNodeEx(matLabel.c_str(), ImGuiTreeNodeFlags_Framed)) {
                    const std::unordered_map<std::string, Uniform>* uniformMap = uniformRegPtr->tryReadUniforms(matID);

                    if (uniformMap == nullptr) {
                        loggerPtr_->addLog(LogLevel::WARNING, "drawUniformInspector", "Model not found in registry: ", std::to_string(modelID));
                        continue;
                    }

                    ImGui::Indent(6.0f);
                    for (auto& [uniformName, uniformRef] : *uniformMap) {
                        ImGui::PushID(imGuiID);
                        Uniform uniformCopy = uniformRef;
                        drawUniformInput(uniformCopy, matID, inspectorEngPtr);
                        ImGui::PopID();
                        imGuiID++;
                    }
                    ImGui::Unindent(6.0f);
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }

    ImGui::PopStyleVar(2);
}

void UniformInspectorUI::drawUniformInput(Uniform& uniform, unsigned int modelID, InspectorEngine* inspectorEngPtr) {
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.22f, 0.25f, 0.29f, 0.55f));
    bool nodeOpen = ImGui::TreeNodeEx(uniform.name.c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowOverlap);
    ImGui::PopStyleColor();

    if (nodeOpen) {
        ImGui::Indent(8.0f);

        bool changed = false;
        bool changedFunctionBox = false;

        ImGui::AlignTextToFramePadding();
        changedFunctionBox |= ImGui::Checkbox("Reference another value", &uniform.isFunction);

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

        ImGui::Spacing();
        std::visit([&](auto& val) {
            changed = drawUniformInputValue(&val, &uniform);
        }, uniform.value);

        if (changed || changedFunctionBox) {
            inspectorEngPtr->applyInput(modelID, uniform);
        }

        ImGui::Unindent(8.0f);
        ImGui::TreePop();
    }
}

bool UniformInspectorUI::drawTextInput(std::string* value, const char* label) {
    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), "%s", value->c_str());
    if (ImGui::InputText(label, buffer, sizeof(buffer))) {
        *value = buffer;
        return true;
    }
    return false;
}

bool UniformInspectorUI::drawUniformInputValue(int* value, Uniform* uniform) {
    ImGui::SetNextItemWidth(kInputWidth);
    return ImGui::DragInt("Value", value);
}

bool UniformInspectorUI::drawUniformInputValue(float* value, Uniform* uniform) {
    ImGui::SetNextItemWidth(kInputWidth);
    return ImGui::DragFloat("Value", value, .1f);
}

bool UniformInspectorUI::drawUniformInputValue(glm::vec3* value, Uniform* uniform) {
    bool changed = false;
    bool useColorPicker = false;
    if (uniform != nullptr) {
        changed |= ImGui::Checkbox("Color picker", &uniform->useAlternateEditor);
        useColorPicker = uniform->useAlternateEditor;
    }

    if (useColorPicker) {
        ImGui::SetNextItemWidth(kColorPickerSize);
        changed |= ImGui::ColorPicker3("##vec3color", &value->x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    } else {
        changed |= ImGui::DragFloat3("xyz", &value->x, .1f);
    }
    return changed;
}

bool UniformInspectorUI::drawUniformInputValue(glm::vec4* value, Uniform* uniform) {
    bool changed = false;
    bool useColorPicker = false;
    if (uniform != nullptr) {
        changed |= ImGui::Checkbox("Color picker", &uniform->useAlternateEditor);
        useColorPicker = uniform->useAlternateEditor;
    }

    if (useColorPicker) {
        ImGui::SetNextItemWidth(kColorPickerSize);
        changed |= ImGui::ColorPicker4("##vec4color", &value->x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar);
    } else {
        changed |= ImGui::DragFloat4("xyzw", &value->x, .1f);
    }
    return changed;
}

bool UniformInspectorUI::drawUniformInputValue(glm::quat* value, Uniform* uniform) {
    ImGui::TextUnformatted("Quaternion (w, x, y, z)");
    ImGui::PushItemWidth(kInputWidth);
    bool changed = false;
    changed |= ImGui::InputFloat("W", &value->w, 0.0f, 0.0f, "%.3f");
    ImGui::SameLine(0.0f, 6.0f);
    changed |= ImGui::InputFloat("X", &value->x, 0.0f, 0.0f, "%.3f");
    ImGui::SameLine(0.0f, 6.0f);
    changed |= ImGui::InputFloat("Y", &value->y, 0.0f, 0.0f, "%.3f");
    ImGui::SameLine(0.0f, 6.0f);
    changed |= ImGui::InputFloat("Z", &value->z, 0.0f, 0.0f, "%.3f");
    ImGui::PopItemWidth();
    return changed;
}

bool UniformInspectorUI::drawUniformInputValue(glm::mat4* value, Uniform* uniform) {
    static int tableID = 0;
    const int columns = 4;
    tableID++;
    ImGui::TextUnformatted("Matrix 4×4");
    bool changed = false;
    if (ImGui::BeginTable(("Matrix4x4" + std::to_string(tableID)).c_str(), columns, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame)) {
        for (int row = 0; row < 4; ++row) {
            ImGui::TableNextRow();
            for (int col = 0; col < 4; ++col) {
                ImGui::TableSetColumnIndex(col);
                ImGui::PushID(row * 4 + col);
                ImGui::SetNextItemWidth(-1);
                changed |= ImGui::InputFloat("##cell", &(*value)[col][row], 0.0f, 0.0f, "%.2f");
                ImGui::PopID();
            }
        }
        ImGui::EndTable();
    }
    return changed;
}

bool UniformInspectorUI::drawUniformInputValue(InspectorSampler2D* value, Uniform* uniform) {
    ImGui::SetNextItemWidth(kInputWidth);
    return ImGui::DragInt("Texture unit", &value->textureUnit, 0, 16);
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
        changed |= ImGui::Checkbox("Use world data", &value->useWorldData);
    }

    int i = 0;
    for (unsigned int matID : materialCachePtr_->getAllMaterialIDs()) {
        if (matID == uniform->materialID) {
            continue;
        }
        modelNames.push_back("Model " + std::to_string(matID));
        modelChoices.push_back(modelNames[i].c_str());
        modelIDs.push_back(matID);
        i++;
    }

    if (value->useWorldData) {
        modelChoices.push_back("Camera");
    }

    ImGui::SetNextItemWidth(-1);
    changed |= ImGui::Combo("Source model", &value->modelSelection, modelChoices.data(), static_cast<int>(modelChoices.size()));

    if (changed) {
        value->uniformSelection = 0;
    }
    if (value->modelSelection == 0) {
        return changed;
    }

    if (std::string(modelChoices[value->modelSelection]) == "Camera") {
        value->useCamaraData = true;
    } else {
        value->referencedMaterialID = modelIDs[value->modelSelection];
    }

    std::vector<const char*> uniformChoices{""};

    if (!value->useWorldData) {
        const auto modelUniforms = uniformRegPtr_->tryReadUniforms(value->referencedMaterialID);
        if (modelUniforms == nullptr) {
            loggerPtr_->addLog(LogLevel::LOG_ERROR, "drawUniformInputValue", "modelID " + std::to_string(value->referencedMaterialID) + " not found in Uniform Registry!");
            return false;
        }

        for (auto& [uniformName, u] : *modelUniforms) {
            if (u.type != value->returnType) {
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

    ImGui::SetNextItemWidth(-1);
    changed |= ImGui::Combo("Source uniform", &value->uniformSelection, uniformChoices.data(), static_cast<int>(uniformChoices.size()));

    if (value->uniformSelection > 0) {
        value->referencedUniformName = uniformChoices[value->uniformSelection];
        value->initialized = true;
    }

    return changed;
}

