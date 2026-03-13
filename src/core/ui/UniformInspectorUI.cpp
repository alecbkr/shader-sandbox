#include "core/ui/UniformInspectorUI.hpp"

#include "core/InspectorEngine.hpp"
#include "core/logging/LogSink.hpp"
#include "core/logging/Logger.hpp"
#include "core/UniformRegistry.hpp"
#include "core/UniformTypes.hpp"
#include "core/ShaderRegistry.hpp"
#include "engine/ShaderProgram.hpp"
#include "imgui.h"
#include "object/MaterialCache.hpp"
#include "object/ModelCache.hpp"
#include <string>
#include <unordered_map>
#include <vector>

void UniformInspectorUI::draw(Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, UniformRegistry* uniformRegPtr, ModelCache* modelCachePtr, MaterialCache* materialCachePtr) {
    loggerPtr_ = loggerPtr;
    uniformRegPtr_ = uniformRegPtr;
    modelCachePtr_ = modelCachePtr;
    materialCachePtr_ = materialCachePtr;
    inspectorEngPtr_ = inspectorEngPtr;
    
    int imGuiID = 0;
    int modelIndex = 0;
    const int modelCount = static_cast<int>(modelCachePtr->getNumberOfModels());

    ImGui::Dummy(ImVec2(0, 3)); // some padding
    for (auto& [modelID, model] : modelCachePtr->modelIDMap) {
        const std::vector<unsigned int>& materialIDs = model->getAllMaterialIDs();
        drawModelContainer(imGuiID, modelID, materialIDs);

        modelIndex++;
        if (modelIndex < modelCount) {
            ImGui::Dummy(ImVec2(0, 2));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0, 2));
        }
    }
}

void UniformInspectorUI::drawModelContainer(int& imGuiID, unsigned int modelID, const std::vector<unsigned int>& materialIDs) {
    std::string modelLabel = "Model " + std::to_string(modelID);

    ImGui::PushID(modelLabel.c_str());

    ImGui::PushStyleColor(ImGuiCol_ChildBg, theme.bgColor);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Header, theme.bgColor);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, theme.bgColorHovered);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, theme.bgColor);

    ImGui::BeginChild("UniformContainer", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);

    std::string headerLabel = modelLabel + "##uniform_model_" + std::to_string(modelID);
    if (ImGui::CollapsingHeader(headerLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth)) {
        drawMaterialContainer(modelID, materialIDs, imGuiID);
    }

    ImGui::EndChild();
    ImGui::PopStyleColor(3);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
    ImGui::PopID();
}

void UniformInspectorUI::drawMaterialContainer(unsigned int modelID, const std::vector<unsigned int>& materialIDs, int& imGuiID) {
    ImGui::Separator();
    ImGui::Indent(theme.indentSize);

    for (unsigned int matID : materialIDs) {
        bool useMaterialHeader = materialIDs.size() > 1;
        bool showUniforms = true;
        if (useMaterialHeader) {
            std::string matHeader = "Material " + std::to_string(matID) + "##uniform_mat_" + std::to_string(matID);
            showUniforms = ImGui::CollapsingHeader(matHeader.c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth);
            if (showUniforms) {
                ImGui::Separator();
            }
        }

        if (showUniforms) {
            const std::unordered_map<std::string, Uniform>* uniformMap = uniformRegPtr_->tryReadUniforms(matID);

            if (uniformMap == nullptr) {
                loggerPtr_->addLog(LogLevel::WARNING, "drawUniformInspector", "Model not found in registry: ", std::to_string(modelID));
                continue;
            }

            ImGui::Indent(theme.indentSize);
            for (auto& [uniformName, uniformRef] : *uniformMap) {
                ImGui::PushID(imGuiID);
                Uniform uniformCopy = uniformRef;
                drawUniformRow(uniformCopy, matID);
                ImGui::PopID();
                imGuiID++;
            }
            ImGui::Unindent(theme.indentSize);
        }
    }

    ImGui::Unindent(theme.indentSize);
}

void UniformInspectorUI::drawUniformRow(Uniform& uniform, unsigned int matID) {
    bool nodeOpen = ImGui::CollapsingHeader(uniform.name.c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowOverlap);

    if (nodeOpen) {
        ImGui::Indent(theme.indentSize);

        bool changed = false;
        bool changedFunctionBox = false;

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Reference Another Value");
        ImGui::SameLine();
        changedFunctionBox |= ImGui::Checkbox("##changedFunctionCheckbox", &uniform.isFunction);

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
                uniform.value = inspectorEngPtr_->getDefaultValue(uniform.type);
            }
        }

        ImGui::Spacing();
        std::visit([&](auto& val) {
            changed = drawInput(&val, &uniform);
        }, uniform.value);

        if (changed || changedFunctionBox) {
            inspectorEngPtr_->applyInput(matID, uniform);
        }

        ImGui::Unindent(theme.indentSize);
    }
}

bool UniformInspectorUI::drawInput(int* value, Uniform* uniform) {
    ImGui::SetNextItemWidth(theme.inputWidth);
    return ImGui::DragInt("##Value", value);
}

bool UniformInspectorUI::drawInput(float* value, Uniform* uniform) {
    ImGui::SetNextItemWidth(theme.inputWidth);
    return ImGui::DragFloat("##Value", value, .1f);
}

bool UniformInspectorUI::drawInput(glm::vec3* value, Uniform* uniform) {
    bool changed = false;
    bool useColorPicker = false;
    if (uniform != nullptr) {
        ImGui::Text("Color Picker");
        ImGui::SameLine();
        changed |= ImGui::Checkbox("##vec3colorOption", &uniform->useAlternateEditor);
        useColorPicker = uniform->useAlternateEditor;
    }

    if (useColorPicker) {
        ImGui::SetNextItemWidth(theme.colorPickerWidthVec3);
        changed |= ImGui::ColorPicker3("##vec3color", &value->x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    } else {
        ImGui::Text("xyz");
        ImGui::SameLine();
        changed |= ImGui::DragFloat3("##xyz", &value->x, .1f);
    }
    return changed;
}

bool UniformInspectorUI::drawInput(glm::vec4* value, Uniform* uniform) {
    bool changed = false;
    bool useColorPicker = false;
    if (uniform != nullptr) {
        ImGui::Text("Color Picker");
        ImGui::SameLine();
        changed |= ImGui::Checkbox("##vec4colorOption", &uniform->useAlternateEditor);
        useColorPicker = uniform->useAlternateEditor;
    }

    if (useColorPicker) {
        ImGui::SetNextItemWidth(theme.colorPickerWidthVec4);
        changed |= ImGui::ColorPicker4("##vec4color", &value->x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar);
    } else {
        ImGui::Text("xyzw");
        ImGui::SameLine();
        changed |= ImGui::DragFloat4("##xyzw", &value->x, .1f);
    }
    return changed;
}

bool UniformInspectorUI::drawInput(glm::quat* value, Uniform* uniform) {
    ImGui::PushItemWidth(theme.inputWidth);
    bool changed = false;
    ImGui::Text("wxyz");
    ImGui::SameLine();
    changed |= ImGui::DragFloat4("##quat", &value->w);
    ImGui::PopItemWidth();
    return changed;
}

bool UniformInspectorUI::drawInput(glm::mat4* value, Uniform* uniform) {
    static int tableID = 0;
    const int columns = 4;
    tableID++;
    bool changed = false;
    if (ImGui::BeginTable(("##Matrix4x4" + std::to_string(tableID)).c_str(), columns, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame)) {
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

bool UniformInspectorUI::drawInput(InspectorSampler2D* value, Uniform* uniform) {
    ImGui::Text("Texture Unit");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(theme.inputWidth);
    return ImGui::DragInt("##Texture_unit", &value->textureUnit, 0, 16);
}

bool UniformInspectorUI::drawInput(InspectorReference* value, Uniform* uniform) {
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
        ImGui::Text("Use World Data?");
        ImGui::SameLine();
        changed |= ImGui::Checkbox("##Use_world_data", &value->useWorldData);
    }

    int i = 0;
    for (unsigned int modelID : modelCachePtr_->getModelIDs()) {
        modelNames.push_back("Model " + std::to_string(modelID));
        modelChoices.push_back(modelNames[i].c_str());
        modelIDs.push_back(modelID);
        i++;
    }

    if (value->useWorldData) {
        modelChoices.push_back("Camera");
    }

    ImGui::Text("Source Model");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(-1);
    changed |= ImGui::Combo("##Source_model", &value->modelSelection, modelChoices.data(), static_cast<int>(modelChoices.size()));

    if (changed) {
        value->materialSelection = 0;
    }
    if (value->modelSelection == 0) {
        return changed;
    }

    if (std::string(modelChoices[value->modelSelection]) == "Camera") {
        value->useCamaraData = true;
    } else {
        value->referencedModelID = modelIDs[value->modelSelection];
    }

    std::vector<const char*> uniformChoices{""};

    if (value->useWorldData) {
        if (!worldData) {
            loggerPtr_->addLog(LogLevel::LOG_ERROR, "drawInput", "world data is not enabled for this type");
            value->modelSelection = 0;
            return false;
        }
        for (const std::string& worldDataStr : worldData.value()) {
            uniformChoices.push_back(worldDataStr.c_str());
        }
    }


    Model* chosenModel = modelCachePtr_->getModel(modelIDs[value->modelSelection]);
    if (!chosenModel) {
        loggerPtr_->addLog(LogLevel::LOG_ERROR, "UniformInspectorUI::drawInput(InspectorReference)", "model not found!");
        value->modelSelection = 0;
        return false;
    }

    auto& chosenModelMatIDs = chosenModel->getAllMaterialIDs();
    std::vector<unsigned int> matIDs{0};

    if (chosenModelMatIDs.size() < 1) {
        loggerPtr_->addLog(LogLevel::LOG_ERROR, "UniformInspectorUI::drawInput(InspectorReference)", "model has no materials!");
        value->modelSelection = 0;
        return false;
    }
    // If model only has one material, we can just show them the uniforms on the only material since there's only one material.
    else if (chosenModelMatIDs.size() < 2) {
        changed = value->materialSelection != 1;
        matIDs.push_back(chosenModelMatIDs.front());
        value->materialSelection = 1;
    }
    else {
        std::vector<std::string> matNames{""};
        std::vector<const char*> matChoices{""};
        matNames.reserve(chosenModelMatIDs.size() + 1);
        matIDs.reserve(chosenModelMatIDs.size() + 1);
        matChoices.reserve(chosenModelMatIDs.size() + 1);
        i = 0;
        for (unsigned int matID : chosenModelMatIDs) {
            matNames.push_back("Material " + std::to_string(matID));
            matChoices.push_back(matNames[i].c_str());
            matIDs.push_back(matID);
            i++;
        }
        ImGui::Text("Source Material");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
        changed |= ImGui::Combo("##Source_material", &value->materialSelection, matChoices.data(), static_cast<int>(matChoices.size()));
    }

    if (changed) {
        value->referencedMaterialID = matIDs[value->materialSelection];
        value->uniformSelection = 0;
    }
    if (value->materialSelection == 0) {
        return changed;
    }

    if (!value->useWorldData) {
        const auto uniforms = uniformRegPtr_->tryReadUniforms(matIDs[value->materialSelection]);
        if (uniforms == nullptr) {
            loggerPtr_->addLog(LogLevel::LOG_ERROR, "UniformInspectorUI::drawInput (function)", "uniform list not found");
        }

        uniformChoices.reserve(uniforms->size() + 1);
        for (const auto& [name, uniform] : *uniforms) {
            if (uniform.type == value->returnType) uniformChoices.push_back(name.c_str());
        }
    }

    ImGui::Text("Source Uniform");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(-1);
    changed |= ImGui::Combo("##Source_uniform", &value->uniformSelection, uniformChoices.data(), static_cast<int>(uniformChoices.size()));

    if (value->uniformSelection > 0) {
        value->referencedUniformName = uniformChoices[value->uniformSelection];
        value->initialized = true;
    }

    return changed;
}

