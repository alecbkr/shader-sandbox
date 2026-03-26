#include "core/ui/UniformInspectorUI.hpp"

#include "application/SettingsStyles.hpp"
#include "core/InspectorEngine.hpp"
#include "core/logging/LogSink.hpp"
#include "core/logging/Logger.hpp"
#include "core/UniformRegistry.hpp"
#include "core/UniformTypes.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/ui/Fonts.hpp"
#include "engine/ShaderProgram.hpp"
#include "imgui.h"
#include "object/MaterialCache.hpp"
#include "object/ModelCache.hpp"
#include <string>
#include <unordered_map>
#include <vector>

UniformInspectorUI::UniformInspectorUI(Fonts* fonts, SettingsStyles* styles) : fonts_(fonts), styles_(styles) {
    if (styles_) {
        theme.bgColor = styles_->assetsFileBackgroundColor;
        // Derive a hover color from the base color so it always differs visibly.
        theme.bgColorHovered = ImVec4(
            theme.bgColor.x * 1.3f,
            theme.bgColor.y * 1.3f,
            theme.bgColor.z * 1.3f,
            theme.bgColor.w
        );
        theme.indentSize = styles_->indentSpacing * 0.5f;
    }
}

void UniformInspectorUI::draw(Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, UniformRegistry* uniformRegPtr, ModelCache* modelCachePtr, MaterialCache* materialCachePtr) {
    loggerPtr_ = loggerPtr;
    uniformRegPtr_ = uniformRegPtr;
    modelCachePtr_ = modelCachePtr;
    materialCachePtr_ = materialCachePtr;
    inspectorEngPtr_ = inspectorEngPtr;
    
    int imGuiID = 0;
    int modelIndex = 0;
    const int modelCount = static_cast<int>(modelCachePtr->getNumberOfModels());
    bool hasActivePrograms = false;

    for (auto& model : modelCachePtr->getAllModels()) {
        for (auto& [matID, matRefCount] : model->getAllMaterialReferences()) {
            Material* material = materialCachePtr_->getMaterial(matID);
            if (material == nullptr) {
                continue;
            }

            ShaderProgram* program = shaderRegPtr->getProgram(material->getProgramID());
            if (program != nullptr && program->isCompiled()) {
                hasActivePrograms = true;
                break;
            }
        }
    }


    ImGui::PushStyleColor(ImGuiCol_ChildBg, styles_->assetsTabBackgroundColor);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    const float window_padding = styles_->assetsBodyPadding;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(window_padding, window_padding));

    if (ImGui::BeginChild("UniformsContent", ImVec2(0, 0), 
                          ImGuiChildFlags_AlwaysUseWindowPadding)) {
    const float inner_padding = styles_->assetsTitleInnerPadding;
        ImGui::PushFont(fonts_->getL4());
        const float directory_height = window_padding * 2 + inner_padding * 2 + ImGui::CalcTextSize("Uniforms").y;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0,0,0,0));

        if (ImGui::BeginChild("UniformsTitle", ImVec2(0, directory_height), 
                              ImGuiChildFlags_AlwaysUseWindowPadding)) {
            ImVec2 p = ImGui::GetWindowPos();
            ImVec2 s = ImGui::GetWindowSize();

            ImGui::GetWindowDrawList()->AddRectFilled(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles_->assetsTitleBackgroundColor),
                styles_->assetsBodyRounding,
                ImDrawFlags_RoundCornersTop
            );

            ImGui::GetWindowDrawList()->AddRect(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles_->assetsBorderColor),
                styles_->assetsBodyRounding,
                ImDrawFlags_RoundCornersTop,
                styles_->assetsBorderThickness
            );

            ImGui::SetCursorPosY(inner_padding + window_padding);
            ImGui::Dummy(ImVec2(styles_->assetsTitleOffset, 0.0f));
            ImGui::SameLine();
            ImGui::TextUnformatted("Uniforms");
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopFont();

        if (ImGui::BeginChild("UniformsBody", ImVec2(0, 0), 
                              ImGuiChildFlags_AlwaysUseWindowPadding)) {
            ImVec2 p = ImGui::GetWindowPos();
            ImVec2 s = ImGui::GetWindowSize();

            ImGui::GetWindowDrawList()->AddRectFilled(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles_->assetsTreeBodyColor),
                styles_->assetsBodyRounding,
                ImDrawFlags_RoundCornersBottom
            );

            ImGui::GetWindowDrawList()->AddRect(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles_->assetsBorderColor),
                styles_->assetsBodyRounding,
                ImDrawFlags_RoundCornersBottom,
                styles_->assetsBorderThickness
            );

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 8.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);

            if (modelCount == 0) {
                ImGui::TextDisabled("No objects");
            } 
            else if (!hasActivePrograms) {
                ImGui::TextDisabled("No active programs");
            }
            else {
                for (auto& model : modelCachePtr->getAllModels()) {
                    const std::unordered_map<unsigned int, unsigned int>& materialReferences = model->getAllMaterialReferences();
                    drawModelContainer(imGuiID, model->ID, materialReferences);

                    modelIndex++;
                    if (modelIndex < modelCount) {
                        ImGui::Separator();
                    }
                }
            }
            ImGui::PopStyleVar(2);
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void UniformInspectorUI::drawModelContainer(int& imGuiID, unsigned int modelID, const std::unordered_map<unsigned int, unsigned int>& materialReferences) {
    std::string modelLabel = "Object " + std::to_string(modelID);

    ImGui::PushID(modelLabel.c_str());

    ImGui::PushStyleColor(ImGuiCol_ChildBg, theme.bgColor);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, styles_->assetsBodyRounding);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, styles_->assetsBorderThickness);
    ImGui::PushStyleColor(ImGuiCol_Header, theme.bgColor);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, theme.bgColorHovered);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, theme.bgColor);

    ImGui::BeginChild("UniformContainer", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);

    std::string headerLabel = modelLabel + "##uniform_model_" + std::to_string(modelID);
    if (ImGui::CollapsingHeader(headerLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth)) {
        drawMaterialContainer(modelID, materialReferences, imGuiID);
    }

    ImGui::EndChild();
    ImGui::PopStyleColor(3);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
    ImGui::PopID();
}

void UniformInspectorUI::drawMaterialContainer(unsigned int modelID, const std::unordered_map<unsigned int, unsigned int>& materialReferences, int& imGuiID) {
    ImGui::Separator();
    ImGui::Indent(theme.indentSize);

    int i = 0;
    for (auto& [matID, matRefCount] : materialReferences) {
        bool useMaterialHeader = materialReferences.size() > 1;
        bool showUniforms = true;
        if (useMaterialHeader) {
            std::string matHeader = "Material " + std::to_string(matID) + "##uniform_mat_" + std::to_string(matID);
            showUniforms = ImGui::CollapsingHeader(matHeader.c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth);
            if (showUniforms) {
                ImGui::Separator();
            }
        }

        if (showUniforms) {
            const auto uniformMap = uniformRegPtr_->tryReadMaterialUniforms(matID);

            if (uniformMap == nullptr) {
                loggerPtr_->addLog(LogLevel::WARNING, "drawUniformInspector", "Model not found in registry: ", std::to_string(modelID));
                continue;
            }

            ImGui::Indent(theme.indentSize);
            drawUniformsNested_byCursor(*uniformMap, matID, imGuiID);
            ImGui::Unindent(theme.indentSize);
        }
        if (i < materialReferences.size()) {
            ImGui::Dummy(ImVec2(0, 4));
        }
    }

    ImGui::Unindent(theme.indentSize);
}

void UniformInspectorUI::drawUniformsNested_byCursor(const std::unordered_map<std::string, Uniform>& uniforms, unsigned int matID, int& imGuiID) {
    struct UniformTreeNode_byCursor {
        const Uniform* uniform = nullptr;
        std::unordered_map<std::string, UniformTreeNode_byCursor> children;
    };

    auto parseSegments_byCursor = [](const std::string& name) -> std::vector<std::string> {
        std::vector<std::string> segments;
        std::string current;
        const std::size_t len = name.size();

        for (std::size_t i = 0; i < len; ++i) {
            char c = name[i];
            if (c == '.') {
                if (!current.empty()) {
                    segments.push_back(current);
                    current.clear();
                }
            } else if (c == '[') {
                if (!current.empty()) {
                    segments.push_back(current);
                    current.clear();
                }
                std::size_t j = i;
                while (j < len && name[j] != ']') {
                    ++j;
                }
                if (j < len && name[j] == ']') {
                    ++j;
                }
                segments.push_back(name.substr(i, j - i));
                i = j - 1;
            } else {
                current.push_back(c);
            }
        }
        if (!current.empty()) {
            segments.push_back(current);
        }
        return segments;
    };

    UniformTreeNode_byCursor root;

    for (const auto& [uniformName, uniformRef] : uniforms) {
        std::vector<std::string> segments = parseSegments_byCursor(uniformName);
        if (segments.empty()) {
            continue;
        }
        UniformTreeNode_byCursor* node = &root;
        for (const std::string& seg : segments) {
            node = &node->children[seg];
        }
        node->uniform = &uniformRef;
    }

    auto renderNode_byCursor = [&](auto&& self, const std::string& segment, const UniformTreeNode_byCursor& node, const std::string& path) -> void {
        const bool hasChildren = !node.children.empty();
        const bool hasUniform = node.uniform != nullptr;

        if (segment.empty()) {
            for (const auto& [childSeg, childNode] : node.children) {
                const std::string childPath = childSeg;
                self(self, childSeg, childNode, childPath);
            }
            return;
        }

        if (!hasChildren) {
            if (hasUniform) {
                ImGui::PushID(imGuiID);
                Uniform uniformCopy = *node.uniform;
                drawUniformRow(uniformCopy, matID);
                ImGui::PopID();
                ++imGuiID;
            }
            return;
        }

        std::string headerLabel = segment + "##group_" + std::to_string(matID) + "_" + path;
        bool open = ImGui::CollapsingHeader(headerLabel.c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth);
        if (!open) {
            return;
        }

        ImGui::Separator();
        ImGui::Indent(theme.indentSize);

        if (hasUniform) {
            ImGui::PushID(imGuiID);
            Uniform uniformCopy = *node.uniform;
            drawUniformRow(uniformCopy, matID);
            ImGui::PopID();
            ++imGuiID;
        }

        for (const auto& [childSeg, childNode] : node.children) {
            const std::string childPath = path.empty() ? childSeg : (path + "." + childSeg);
            self(self, childSeg, childNode, childPath);
        }

        ImGui::Unindent(theme.indentSize);
    };

    for (const auto& [seg, childNode] : root.children) {
        renderNode_byCursor(renderNode_byCursor, seg, childNode, seg);
    }
}

void UniformInspectorUI::drawUniformRow(Uniform& uniform, unsigned int matID) {
    // I think it would be better to maybe just say it's here and have it be read only? invisible isn't a great idea imo
    if (uniform.invisible) return;

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
    modelNames.reserve(modelCachePtr_->getNumberOfModels());
    modelChoices.reserve(modelCachePtr_->getNumberOfModels() + 1);
    modelIDs.reserve(modelCachePtr_->getNumberOfModels() + 1);

    std::optional<std::vector<std::string>> worldData = getWorldData(uniform->type);

    if (worldData || value->useWorldData) {
        ImGui::Text("Use World Data?");
        ImGui::SameLine();
        changed |= ImGui::Checkbox("##Use_world_data", &value->useWorldData);
    }

    int i = 0;
    for (auto& model : modelCachePtr_->getAllModels()) {
        modelNames.push_back("Object " + std::to_string(model->ID));
        modelChoices.push_back(modelNames[i].c_str());
        modelIDs.push_back(model->ID);
        i++;
    }

    if (value->useWorldData) {
        modelChoices.push_back("Camera");
    }

    ImGui::Text("Source Object");
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

    //++++++++++++++++START

    // Model* chosenModel = modelCachePtr_->getModel(modelIDs[value->modelSelection]);
    // if (!chosenModel) {
    //     loggerPtr_->addLog(LogLevel::LOG_ERROR, "UniformInspectorUI::drawInput(InspectorReference)", "model not found!");
    //     value->modelSelection = 0;
    //     return false;
    // }

    // auto& chosenModelMatIDReferences = chosenModel->getAllMaterialReferences();
    // std::vector<unsigned int> matIDs{0};

    // if (chosenModelMatIDReferences.size() < 1) {
    //     loggerPtr_->addLog(LogLevel::LOG_ERROR, "UniformInspectorUI::drawInput(InspectorReference)", "model has no materials!");
    //     value->modelSelection = 0;
    //     return false;
    // }
    // // If model only has one material, we can just show them the uniforms on the only material since there's only one material.
    // else if (chosenModelMatIDReferences.size() < 2) {
    //     changed = value->materialSelection != 1;
    //     auto& [matID, matRefCount] = *chosenModelMatIDReferences.begin(); //gets the only entry
    //     matIDs.push_back(matID);
    //     value->materialSelection = 1;
    // }
    // else {
    //     std::vector<std::string> matNames{""};
    //     std::vector<const char*> matChoices{""};
    //     matNames.reserve(chosenModelMatIDReferences.size() + 1);
    //     matIDs.reserve(chosenModelMatIDReferences.size() + 1);
    //     matChoices.reserve(chosenModelMatIDReferences.size() + 1);
    //     i = 0;
    //     for (auto& [matID, matRefCount] : chosenModelMatIDReferences) {
    //         matNames.push_back("Material " + std::to_string(matID));
    //         matChoices.push_back(matNames[i].c_str());
    //         matIDs.push_back(matID);
    //         i++;
    //     }
    //     ImGui::Text("Source Material");
    //     ImGui::SameLine();
    //     ImGui::SetNextItemWidth(-1);
    //     changed |= ImGui::Combo("##Source_material", &value->materialSelection, matChoices.data(), static_cast<int>(matChoices.size()));
    // }

    // if (changed) {
    //     value->referencedMaterialID = matIDs[value->materialSelection];
    //     value->uniformSelection = 0;
    // }
    // if (value->materialSelection == 0) {
    //     return changed;
    // }

    //+++++++++++++++++++++END

    if (!value->useWorldData) {
        Model* chosenModel = modelCachePtr_->getModel(modelIDs[value->modelSelection]);
        if (!chosenModel) {
            loggerPtr_->addLog(LogLevel::LOG_ERROR, "UniformInspectorUI::drawInput(InspectorReference)", "model not found!");
            value->modelSelection = 0;
            return false;
        }

        auto& chosenModelMatReferences = chosenModel->getAllMaterialReferences();
        std::vector<unsigned int> matIDs{0};

        if (chosenModelMatReferences.size() < 1) {
            loggerPtr_->addLog(LogLevel::LOG_ERROR, "UniformInspectorUI::drawInput(InspectorReference)", "model has no materials!");
            value->modelSelection = 0;
            return false;
        }
        // If model only has one material, we can just show them the uniforms on the only material since there's only one material.
        else if (chosenModelMatReferences.size() < 2) {
            changed = value->materialSelection != 1;
            auto& [matID, matRefCount] = *chosenModelMatReferences.begin(); //same as getting front, retrieves only entry
            matIDs.push_back(matID);
            value->materialSelection = 1;
        }
        else {
            std::vector<std::string> matNames{""};
            std::vector<const char*> matChoices{""};
            matNames.reserve(chosenModelMatReferences.size() + 1);
            matIDs.reserve(chosenModelMatReferences.size() + 1);
            matChoices.reserve(chosenModelMatReferences.size() + 1);
            i = 0;
            for (auto& [matID, matRefCount] : chosenModelMatReferences) {
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

        const auto uniforms = uniformRegPtr_->tryReadMaterialUniforms(matIDs[value->materialSelection]);
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

