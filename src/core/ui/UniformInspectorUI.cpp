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
#include <sstream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <vector>

namespace {
    const std::string worldVariableLabel = "World Variable";
}

bool UniformInspectorUI::drawCompactTreeNode(const std::string& label) {

    // Set the header button color
    ImGui::PushStyleColor(ImGuiCol_Header, theme.headerColor);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, theme.headerColorHovered);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, theme.headerColor);
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, theme.indentSize); 
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f)); 

    const bool open = ImGui::TreeNodeEx(
        label.c_str(),
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed
    );

    ImGui::PopStyleColor(3); // pop header colors
    ImGui::PopStyleVar(2);

    return open;
}

UniformInspectorUI::UniformInspectorUI(Fonts* fonts, SettingsStyles* styles) : fonts_(fonts), styles_(styles) {
    if (styles_) {
        theme.cardBGColor = styles_->assetsFileBackgroundColor;
        theme.headerColor = styles_->assetsTitleBackgroundColor;
        // Derive a hover color from the base color so it always differs visibly.
        theme.headerColorHovered = ImVec4(
            theme.headerColor.x * 1.2f,
            theme.headerColor.y * 1.2f,
            theme.headerColor.z * 1.2f,
            theme.headerColor.w
        );
        theme.indentSize = styles_->assetsTitleInnerPadding + 2.0f;
    }
}

UniformInspectorUI::~UniformInspectorUI() = default;

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

        if (ImGui::BeginChild("UniformsTree", ImVec2(0, 0), 
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
                        ImGui::Dummy(ImVec2(0.0f, 6.0f));
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
    Model* model = modelCachePtr_->getModel(modelID);
    std::string modelLabel = model ? model->getName() : "Object " + std::to_string(modelID);

    ImGui::PushID(modelLabel.c_str());

    ImGui::PushStyleColor(ImGuiCol_ChildBg, theme.cardBGColor);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, styles_->assetsBodyRounding);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, styles_->assetsBorderThickness);

    ImGui::PushStyleColor(ImGuiCol_Header, theme.headerColor);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, theme.headerColorHovered);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, theme.headerColor);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 4.0f));

    ImGui::BeginChild("UniformContainer", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding);

    std::string headerLabel = modelLabel + "##uniform_model_" + std::to_string(modelID);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 4.0f));
    if (ImGui::CollapsingHeader(headerLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth)) {
        drawMaterialContainer(modelID, materialReferences, imGuiID);

    }
    ImGui::PopStyleVar();

    ImGui::EndChild();
    ImGui::PopStyleColor(3);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);
    ImGui::PopID();
}

void UniformInspectorUI::drawMaterialContainer(unsigned int modelID, const std::unordered_map<unsigned int, unsigned int>& materialReferences, int& imGuiID) {
    ImGui::Indent(theme.indentSize);

    size_t i = 0;
    for (auto& [matID, matRefCount] : materialReferences) {
        bool useMaterialHeader = materialReferences.size() > 1;
        bool showUniforms = true;
        bool mustTreePop = false;
        if (useMaterialHeader) {
            std::string matHeader = "Material " + std::to_string(matID) + "##uniform_mat_" + std::to_string(matID);
            showUniforms = mustTreePop = drawCompactTreeNode(matHeader);
            if (showUniforms) {
                ImGui::Dummy(ImVec2(0.0f, 2.0f));
            }
        }

        if (showUniforms) {
            const auto uniformMap = uniformRegPtr_->tryReadMaterialUniforms(matID);

            if (uniformMap == nullptr) {
                loggerPtr_->addLog(LogLevel::WARNING, "drawMaterialContainer", "Material uniforms not found in registry for material: ", std::to_string(matID));
                continue;
            }

            drawUniformsNested_byCursor(*uniformMap, matID, imGuiID);
        }
        ++i;
        if (i < materialReferences.size()) {
            ImGui::Dummy(ImVec2(0.0f, 4.0f));
        }
        if (mustTreePop) ImGui::TreePop();
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
            if (hasUniform || !node.uniform->invisible) {
                ImGui::PushID(imGuiID);
                Uniform uniformCopy = *node.uniform;
                drawUniformRow(uniformCopy, matID);
                ImGui::PopID();
                ++imGuiID;
            }
            return;
        }

        // Determine label
        const std::string groupTypeLabel =
            (!segment.empty() && segment[0] == '[') ? "Array" : "Struct";

        std::string label = groupTypeLabel + " " + segment + "##uniform_";

        ImGui::PushStyleColor(ImGuiCol_ChildBg, theme.cardBGColor); 
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, styles_->assetsBodyRounding);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, styles_->assetsBorderThickness);
        ImGui::BeginChild(("Group##" + label).c_str(), ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding);

        if (drawCompactTreeNode(label)) {
            ImGui::Dummy(ImVec2(0, .5));
            ImGui::Separator();
            for (const auto& [childSeg, childNode] : node.children) {
                const std::string childPath = path.empty() ? childSeg : (path + "." + childSeg);
                self(self, childSeg, childNode, childPath);
            }

            ImGui::TreePop();
        }
        ImGui::EndChild();
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(1);
    };

    for (const auto& [seg, childNode] : root.children) {
        renderNode_byCursor(renderNode_byCursor, seg, childNode, seg);
    }
}

bool UniformInspectorUI::drawModePicker(const char* id, int& mode, const char* const* labels, int labelCount) {
    bool changed = false;
    static constexpr const char* compactLabels[] = { "Constant", "Reference" };
    const char* buttonLabel = (labelCount == 2 && mode >= 0 && mode < 2) ? compactLabels[mode] : labels[mode];

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 0.0f));
    if (ImGui::SmallButton((std::string(buttonLabel) + "##" + id).c_str())) {
        ImGui::OpenPopup((std::string("ModePopup##") + id).c_str());
    }

    if (ImGui::BeginPopup((std::string("ModePopup##") + id).c_str())) {
        for (int i = 0; i < labelCount; ++i) {
            if (ImGui::MenuItem(labels[i], nullptr, mode == i)) {
                mode = i;
                changed = true;
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    return changed;
}

void UniformInspectorUI::setReferenceMode(Uniform& uniform, bool useReference) {
    if (useReference) {
        if (!std::holds_alternative<InspectorReference>(uniform.value)) {
            uniform.value = InspectorReference{
                .modelSelection = 0,
                .uniformSelection = 0,
                .returnType = uniform.type,
                .initialized = false,
            };
        }
        uniform.isFunction = true;
        return;
    }

    uniform.isFunction = false;
    uniform.value = inspectorEngPtr_->getDefaultValue(uniform.type);
}

std::string UniformInspectorUI::makeUniformStateKey(unsigned int matID, const std::string& uniformName) const {
    return std::to_string(matID) + "::" + uniformName;
}

std::string UniformInspectorUI::getReferenceSummary(const Uniform& uniform) const {
    const auto* ref = std::get_if<InspectorReference>(&uniform.value);
    if (ref == nullptr || !uniform.isFunction) {
        return "Constant";
    }

    if (!ref->initialized) {
        return "Reference: Unconfigured";
    }

    std::string target;
    if (ref->useWorldVariable) {
        target = worldVariableLabel;
    } else if (ref->referencedModelID != 0) {
        Model* model = modelCachePtr_->getModel(ref->referencedModelID);
        target = model ? model->getName() : "Object " + std::to_string(ref->referencedModelID);    } else {
        target = "Reference";
    }

    if (!ref->referencedUniformName.empty()) {
        target += " -> " + ref->referencedUniformName;
    }

    return target;
}

std::string UniformInspectorUI::getUniformSummary(const Uniform& uniform) const {
    if (uniform.isFunction) {
        return getReferenceSummary(uniform);
    }

    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2);

    return std::visit([&](const auto& value) -> std::string {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, int>) {
            return std::to_string(value);
        } else if constexpr (std::is_same_v<T, float>) {
            stream.str("");
            stream.clear();
            stream << value;
            return stream.str();
        } else if constexpr (std::is_same_v<T, glm::vec3>) {
            stream.str("");
            stream.clear();
            stream << "(" << value.x << ", " << value.y << ", " << value.z << ")";
            return stream.str();
        } else if constexpr (std::is_same_v<T, glm::vec4>) {
            stream.str("");
            stream.clear();
            stream << "(" << value.x << ", " << value.y << ", " << value.z << ", " << value.w << ")";
            return stream.str();
        } else if constexpr (std::is_same_v<T, glm::mat4>) {
            return "Matrix";
        } else if constexpr (std::is_same_v<T, InspectorSampler2D>) {
            return "Texture unit " + std::to_string(value.textureUnit);
        } else if constexpr (std::is_same_v<T, InspectorReference>) {
            return getReferenceSummary(uniform);
        }
        return "Value";
    }, uniform.value);
}

void UniformInspectorUI::drawUniformRow(Uniform& uniform, unsigned int matID) {
    bool changed = false;
    int mode = uniform.isFunction ? 1 : 0;
    static constexpr const char* modeLabels[] = { "Constant", "Reference" };
    const std::string typeSummary = to_string(uniform.type);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, theme.cardBGColor); // same card background as model card
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, styles_->assetsBodyRounding);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, styles_->assetsBorderThickness);

    if (ImGui::BeginChild(("UniformCard##" + uniform.name).c_str(), ImVec2(0, 0),
                          ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders)) {
        
        std::string label = typeSummary + " " + uniform.name + "##uniform_";
        bool open = drawCompactTreeNode(label);

        if (open) {
            ImGui::Dummy(ImVec2(0, .5));
            ImGui::Separator();

            ImGui::TextDisabled("Mode:  ");
            ImGui::SameLine();
            if (drawModePicker("InlineMode", mode, modeLabels, IM_ARRAYSIZE(modeLabels))) {
                setReferenceMode(uniform, mode == 1);
                changed = true;
            }

            // Value editor
            if (!uniform.isFunction) {
                std::visit([&](auto& val) {
                    changed |= drawInput(&val, &uniform);
                }, uniform.value);
            } else if (auto* value = std::get_if<InspectorReference>(&uniform.value)) {
                changed |= drawReferenceEditor(value, &uniform);
            }
            ImGui::Dummy(ImVec2(0, .5));
            ImGui::TreePop();
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    // --- End uniform card ---

    if (changed) {
        inspectorEngPtr_->applyInput(matID, uniform);
    }
}

bool UniformInspectorUI::drawInput(int* value, Uniform* uniform) {
    ImGui::SetNextItemWidth(-1);
    return ImGui::DragInt("##Value", value);
}

bool UniformInspectorUI::drawInput(float* value, Uniform* uniform) {
    ImGui::SetNextItemWidth(-1);
    return ImGui::DragFloat("##Value", value, .1f);
}

bool UniformInspectorUI::drawInput(glm::vec3* value, Uniform* uniform) {
    bool changed = false;
    bool useColorPicker = false;
    if (uniform != nullptr) {
        changed |= ImGui::Checkbox("Color Picker##vec3colorOption", &uniform->useAlternateEditor);
        useColorPicker = uniform->useAlternateEditor;
    }

    if (useColorPicker) {
        ImGui::SetNextItemWidth(theme.colorPickerWidthVec3);
        changed |= ImGui::ColorPicker3("##vec3color", &value->x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    } else {
        ImGui::TextDisabled("xyz");
        ImGui::SetNextItemWidth(-1);
        changed |= ImGui::DragFloat3("##xyz", &value->x, .1f);
    }
    return changed;
}

bool UniformInspectorUI::drawInput(glm::vec4* value, Uniform* uniform) {
    bool changed = false;
    bool useColorPicker = false;
    if (uniform != nullptr) {
        changed |= ImGui::Checkbox("Color Picker##vec4colorOption", &uniform->useAlternateEditor);
        useColorPicker = uniform->useAlternateEditor;
    }

    if (useColorPicker) {
        ImGui::SetNextItemWidth(theme.colorPickerWidthVec4);
        changed |= ImGui::ColorPicker4("##vec4color", &value->x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar);
    } else {
        ImGui::TextDisabled("xyzw");
        ImGui::SetNextItemWidth(-1);
        changed |= ImGui::DragFloat4("##xyzw", &value->x, .1f);
    }
    return changed;
}

bool UniformInspectorUI::drawInput(glm::quat* value, Uniform* uniform) {
    bool changed = false;
    ImGui::TextDisabled("wxyz");
    ImGui::SetNextItemWidth(-1);
    changed |= ImGui::DragFloat4("##quat", &value->w);
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
    ImGui::TextDisabled("Texture Unit");
    ImGui::SetNextItemWidth(-1);
    return ImGui::DragInt("##Texture_unit", &value->textureUnit, 0, 16);
}

bool UniformInspectorUI::drawInput(InspectorReference* value, Uniform* uniform) {
    return drawReferenceEditor(value, uniform);
}

bool UniformInspectorUI::drawReferenceEditor(InspectorReference* value, Uniform* uniform) {
    if (value->materialSelection < 0 || value->modelSelection < 0 || value->uniformSelection < 0) {
        value->materialSelection = 0;
        value->modelSelection = 0;
        value->uniformSelection = 0;
        loggerPtr_->addLog(LogLevel::LOG_ERROR, "UniformInspectorUI::drawReferenceEditor", "selection outside of bounds!");
    }
    value->initialized = false;
    value->useWorldVariable = false;
    bool changed = false;
    std::vector<std::string> modelNames;
    std::vector<const char*> modelChoices{""};
    std::vector<unsigned int> modelIDs{0};
    modelNames.reserve(modelCachePtr_->getNumberOfModels());
    modelChoices.reserve(modelCachePtr_->getNumberOfModels() + 1);
    modelIDs.reserve(modelCachePtr_->getNumberOfModels() + 1);

    std::optional<std::vector<std::string>> worldData = getWorldData(uniform->type);

    if (worldData || value->useWorldData) {
        bool changedWorldDataBox = ImGui::Checkbox("Use World Data##Use_world_data", &value->useWorldData);
        if (changedWorldDataBox) {
            value->materialSelection = 0;
            value->modelSelection = 0;
            value->uniformSelection = 0;
        }
        changed |= changedWorldDataBox;
        
    }

    int i = 0;
    for (auto& model : modelCachePtr_->getAllModels()) {
        modelNames.push_back(model->getName());
        modelChoices.push_back(modelNames[i].c_str());
        modelIDs.push_back(model->ID);
        i++;
    }

    if (value->useWorldData) {
        modelChoices.push_back(worldVariableLabel.c_str());
    }


    ImGui::TextDisabled("Source Object");
    ImGui::SetNextItemWidth(-1);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(styles_->framePadding.x, 0.0f));
    changed |= ImGui::Combo("##Source_model", &value->modelSelection, modelChoices.data(), static_cast<int>(modelChoices.size()));
    ImGui::PopStyleVar();

    if (changed) {
        value->materialSelection = 0;
    }
    if (value->modelSelection == 0) {
        return changed;
    }

    if (std::string(modelChoices[value->modelSelection]) == worldVariableLabel) {
        value->useWorldVariable = true;
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
            ImGui::TextDisabled("Source Material");
            ImGui::SetNextItemWidth(-1);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(styles_->framePadding.x, 0.0f));
            changed |= ImGui::Combo("##Source_material", &value->materialSelection, matChoices.data(), static_cast<int>(matChoices.size()));
            ImGui::PopStyleVar();
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

    ImGui::TextDisabled("Source Uniform");
    ImGui::SetNextItemWidth(-1);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(styles_->framePadding.x, 0.0f));
    changed |= ImGui::Combo("##Source_uniform", &value->uniformSelection, uniformChoices.data(), static_cast<int>(uniformChoices.size()));
    ImGui::PopStyleVar();

    if (value->uniformSelection > 0) {
        value->referencedUniformName = uniformChoices[value->uniformSelection];
        value->initialized = true;
    }

    return changed;
}

