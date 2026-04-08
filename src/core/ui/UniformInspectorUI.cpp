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
#include "texture/TextureCache.hpp"
#include <sstream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <vector>

bool UniformInspectorUI::drawCompactTreeNode(const std::string& label) {

    // Set the header button color
    ImGui::PushStyleColor(ImGuiCol_Header, theme.headerColor);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, theme.headerColorHovered);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, theme.headerColor);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    const bool open = ImGui::TreeNodeEx(
        label.c_str(),
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed
    );

    ImGui::PopStyleColor(3); // pop header colors
    ImGui::PopStyleVar(1);

    return open;
}

UniformInspectorUI::UniformInspectorUI(Fonts* fonts, SettingsStyles* styles, Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, UniformRegistry* uniformRegPtr, ModelCache* modelCachePtr, MaterialCache* materialCachePtr, TextureCache* textureCachePtr) : fonts_(fonts), styles_(styles) {
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

    loggerPtr_ = loggerPtr;
    uniformRegPtr_ = uniformRegPtr;
    modelCachePtr_ = modelCachePtr;
    materialCachePtr_ = materialCachePtr;
    shaderRegPtr_ = shaderRegPtr;
    inspectorEngPtr_ = inspectorEngPtr;
    textureCachePtr_ = textureCachePtr;
}

UniformInspectorUI::~UniformInspectorUI() = default;

void UniformInspectorUI::draw() {
    inspectorEngPtr_->queueUpdateChoices();
    
    int imGuiID = 0;
    int modelIndex = 0;
    const int modelCount = static_cast<int>(modelCachePtr_->getNumberOfModels());
    bool hasActivePrograms = false;

    for (auto& model : modelCachePtr_->getAllModels()) {
        for (auto& [matID, matRefCount] : model->getAllMaterialReferences()) {
            Material* material = materialCachePtr_->getMaterial(matID);
            if (material == nullptr) {
                continue;
            }

            ShaderProgram* program = shaderRegPtr_->getProgram(material->getProgramID());
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
                for (auto& model : modelCachePtr_->getAllModels()) {
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
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, theme.indentSize);
    if (drawCompactTreeNode(headerLabel.c_str())) {
        drawMaterialContainer(modelID, materialReferences, imGuiID);
        ImGui::TreePop();
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    ImGui::EndChild();
    ImGui::PopStyleColor(3);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);
    ImGui::PopID();
}

void UniformInspectorUI::drawMaterialContainer(unsigned int modelID, const std::unordered_map<unsigned int, unsigned int>& materialReferences, int& imGuiID) {
    size_t i = 0;
    bool useMaterialHeader = materialReferences.size() > 1;
    for (auto& [matID, matRefCount] : materialReferences) {
        bool showUniforms = true;
        bool mustTreePop = false;
        if (useMaterialHeader) {
            ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, theme.indentSize);
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
            } else {
                drawUniformsNested_byCursor(*uniformMap, matID, imGuiID);
            }
        }
        ++i;
        if (mustTreePop) {
            ImGui::TreePop();
        }
        if (useMaterialHeader) {
            ImGui::PopStyleVar();
        }
        if (i < materialReferences.size()) {
            ImGui::Dummy(ImVec2(0.0f, 4.0f));
        }
    }
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

        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, theme.indentSize);
        if (drawCompactTreeNode(label)) {
            ImGui::Dummy(ImVec2(0, .5));
            ImGui::Separator();
            for (const auto& [childSeg, childNode] : node.children) {
                const std::string childPath = path.empty() ? childSeg : (path + "." + childSeg);
                self(self, childSeg, childNode, childPath);
            }

            ImGui::TreePop();
        }
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(1);
    };

    for (const auto& [seg, childNode] : root.children) {
        renderNode_byCursor(renderNode_byCursor, seg, childNode, seg);
    }
}

bool UniformInspectorUI::drawReferenceModePicker(bool *isRef) {
    bool changed = false;
    const size_t labelCount = 2;

    const char* compactLabels[labelCount] = { "Constant", "Reference" };
    const char* buttonLabel = *isRef ? compactLabels[1] : compactLabels[0];

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 0.0f));

    if (ImGui::SmallButton((std::string(buttonLabel) + "##inlineMode").c_str())) {
        ImGui::OpenPopup("ModePopup##inlineMode");
    }

    if (ImGui::BeginPopup("ModePopup##inlineMode")) {
        for (int i = 0; i < labelCount; ++i) {
            bool selected = (*isRef == (i == 1));

            if (ImGui::MenuItem(compactLabels[i], nullptr, selected)) {
                *isRef = (i == 1);
                changed = true;
            }
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
    return changed;
}

bool UniformInspectorUI::drawReferenceTypePicker(InspectorReferenceType* referenceType) {
    bool changed = false;

    const char* labels[] = { "Uniform", "Object Data", sceneVariableLabel.c_str() };
    const int count = 3;

    int currentIndex = static_cast<int>(*referenceType);
    const char* buttonLabel = labels[currentIndex];

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 0.0f));

    if (ImGui::SmallButton((std::string(buttonLabel) + "##refType").c_str())) {
        ImGui::OpenPopup("RefTypePopup##refType");
    }

    if (ImGui::BeginPopup("RefTypePopup##refType")) {
        for (int i = 0; i < count; ++i) {
            bool selected = (currentIndex == i);

            if (ImGui::MenuItem(labels[i], nullptr, selected)) {
                *referenceType = static_cast<InspectorReferenceType>(i);
                changed = true;
            }
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
    return changed;
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
    switch (ref->referenceType) {
        case InspectorReferenceType::Uniform: 
            target = "Uniform";
            break;
        case InspectorReferenceType::ObjectData:
            if (ref->referencedModelID != 0) {
                target = "Object " + std::to_string(ref->referencedModelID);
            } else {
                target = "Object";
            }
            break;
        case InspectorReferenceType::SceneVariable:
            target = sceneVariableLabel;
            break;
    }

    if (!ref->referencedValueName.empty()) {
        target += " -> " + ref->referencedValueName;
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
    const std::string typeSummary = to_string(uniform.type);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, theme.cardBGColor); // same card background as model card
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, styles_->assetsBodyRounding);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, styles_->assetsBorderThickness);

    if (ImGui::BeginChild(("UniformCard##" + uniform.name).c_str(), ImVec2(0, 0),
                          ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders)) {
        
        std::string label = typeSummary + " " + uniform.name + "##uniform_";
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, theme.indentSize);
        bool open = drawCompactTreeNode(label);

        if (open) {
            ImGui::Dummy(ImVec2(0, .5));
            ImGui::Separator();

            ImGui::TextDisabled("Mode:  ");
            ImGui::SameLine();
            if (drawReferenceModePicker(&uniform.isFunction)) {
                changed = true;
                if (uniform.isFunction) {
                    uniform.value = InspectorReference{
                        .returnType = uniform.type,
                        .initialized = false
                    };
                }
                else {
                    uniform.value = inspectorEngPtr_->getDefaultValue(uniform.type);
                }
            }

            Material* mat = materialCachePtr_->getMaterial(matID);

            // Value editor
            if (!uniform.isFunction) {
                std::visit([&](auto& val) {
                    changed |= drawInput(&val, &uniform, mat);
                }, uniform.value);
            } else if (auto* value = std::get_if<InspectorReference>(&uniform.value)) {
                changed |= drawInput(value, &uniform, mat);
            }
            ImGui::Dummy(ImVec2(0, .5));
            ImGui::TreePop();
        }
        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    // --- End uniform card ---

    if (changed) {
        inspectorEngPtr_->applyInput(matID, uniform);
    }
}

bool UniformInspectorUI::drawInput(int* value, Uniform* uniform, Material* mat) {
    ImGui::SetNextItemWidth(-1);
    return ImGui::DragInt("##Value", value);
}

bool UniformInspectorUI::drawInput(float* value, Uniform* uniform, Material* mat) {
    ImGui::SetNextItemWidth(-1);
    return ImGui::DragFloat("##Value", value, .1f);
}

bool UniformInspectorUI::drawInput(glm::vec3* value, Uniform* uniform, Material* mat) {
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

bool UniformInspectorUI::drawInput(glm::vec4* value, Uniform* uniform, Material* mat) {
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

bool UniformInspectorUI::drawInput(glm::quat* value, Uniform* uniform, Material* mat) {
    bool changed = false;
    ImGui::TextDisabled("wxyz");
    ImGui::SetNextItemWidth(-1);
    changed |= ImGui::DragFloat4("##quat", &value->w);
    return changed;
}

bool UniformInspectorUI::drawInput(glm::mat4* value, Uniform* uniform, Material* mat) {
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

std::string makeRelativeToAssetsFolder(const std::string& fullPath) {
    std::string key = "assets\\";
    size_t pos = fullPath.find(key);

    if (pos != std::string::npos) {
        return fullPath.substr(pos + key.length());
    }

    key = "assets/";
    pos = fullPath.find(key);

    if (pos != std::string::npos) {
        return fullPath.substr(pos + key.length());
    }

    return fullPath;
}

bool UniformInspectorUI::drawInput(InspectorSampler2D* value, Uniform* uniform, Material* material) {
    if (!value || !material || !textureCachePtr_) {
        ImGui::TextDisabled("No textures available");
        return false;
    }

    std::vector<std::string> texPaths = material->getAllTexturePaths(textureCachePtr_);

    ImGui::TextDisabled("Texture Unit");
    ImGui::SetNextItemWidth(-1);

    if (texPaths.empty()) {
        ImGui::TextDisabled("This material has no textures");
        return false;
    }

    bool changed = false;

    std::string preview;
    if (value->textureUnit >= 0 && value->textureUnit < static_cast<int>(texPaths.size())) {
        preview = "Unit " + std::to_string(value->textureUnit) + " | " + makeRelativeToAssetsFolder(texPaths[value->textureUnit]);
    } else {
        preview = "None";
    }

    if (ImGui::BeginCombo("##Texture_unit", preview.c_str())) {
        for (int i = 0; i < static_cast<int>(texPaths.size()); i++) {
            std::string label = "Unit " + std::to_string(i) + " | " + makeRelativeToAssetsFolder(texPaths[i]);
            bool selected = (value->textureUnit == i);

            if (ImGui::Selectable(label.c_str(), selected)) {
                value->textureUnit = i;
                changed = true;
            }

            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    return changed;
}

bool UniformInspectorUI::drawInput(InspectorReference* value, Uniform* uniform, Material* mat) {
    // validation
    if (value->materialSelection < 0 || value->modelSelection < 0 || value->valueSelection < 0) {
        value->resetSelections();
        loggerPtr_->addLog(LogLevel::LOG_ERROR, "UniformInspectorUI::drawInput (InspectorReference)", "selection outside of bounds! Resetting value");
    }

    ImGui::TextDisabled("Reference Type:  ");
    ImGui::SameLine();
    bool changed = false;
    changed = drawReferenceTypePicker(&value->referenceType);

    if (changed) {
        value->resetSelections();
        return true;
    }

    switch (value->referenceType) {
        case InspectorReferenceType::ObjectData:
            changed = drawRefInput_ObjectData(value, uniform);
            break;
        case InspectorReferenceType::Uniform:
            changed = drawRefInput_Uniform(value, uniform);
            break;
        case InspectorReferenceType::SceneVariable:
            changed = drawRefInput_SceneVar(value, uniform);
            break;
    }

    return changed;

}

bool UniformInspectorUI::drawRefInput_Uniform(InspectorReference* value, Uniform* uniform) {
    if (value->referenceType != InspectorReferenceType::Uniform) {
        loggerPtr_->addLog(LogLevel::LOG_ERROR, "UniformInspectorUI::drawRefInput_Uniform", "Wrong reference type! not drawing this");
        return false;
    }

    value->initialized = false;
    bool changed = false;

    auto modelChoices = inspectorEngPtr_->getModelChoices();
    ImGui::TextDisabled("Source Object");
    ImGui::SetNextItemWidth(-1);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(styles_->framePadding.x, 0.0f));
    changed |= ImGui::Combo("##Source_model", &value->modelSelection, modelChoices.cstrings.data(), modelChoices.cstrings.size());
    ImGui::PopStyleVar();

    if (changed) {
        value->materialSelection = 0;
    }
    if (value-> modelSelection >= modelChoices.ids.size()) {
        value->modelSelection = 0;
    }
    if (value->modelSelection == 0) {
        return changed;
    }

    unsigned int chosenID = modelChoices.ids[value->modelSelection];
    auto matChoicesOptional = inspectorEngPtr_->getMatChoices(chosenID);
    if (!matChoicesOptional) {
        loggerPtr_->addLog(LogLevel::LOG_ERROR, "UniformInspectorUI::drawInput(InspectorReference)", "model not found!");
        value->modelSelection = 0;
        return false;
    }

    auto matChoices = matChoicesOptional.value();
    int matCount = matChoices->ids.size() - 1;
    if (matCount < 1) {
        loggerPtr_->addLog(LogLevel::LOG_ERROR, "UniformInspectorUI::drawInput(InspectorReference)", "model has no materials!");
        value->modelSelection = 0;
        return false;
    }
    // If model only has one material, we can just show them the uniforms on the only material since there's only one material.
    else if (matCount < 2) {
        changed = value->materialSelection != 1;
        value->materialSelection = 1;
    }
    else {
        ImGui::TextDisabled("Source Material");
        ImGui::SetNextItemWidth(-1);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(styles_->framePadding.x, 0.0f));
        changed |= ImGui::Combo("##Source_material", &value->materialSelection, matChoices->cstrings.data(), static_cast<int>(matChoices->cstrings.size()));
        ImGui::PopStyleVar();
    }

    if (value->materialSelection >= matChoices->ids.size()) {
        value->materialSelection = 0;
    }
    if (changed) {
        value->referencedMaterialID = matChoices->ids[value->materialSelection];
        value->valueSelection = 0;
    }
    if (value->materialSelection == 0) {
        return changed;
    }

    auto uniformChoicesOptional = inspectorEngPtr_->getUniformChoices(value->referencedMaterialID, value->returnType);
    if (!uniformChoicesOptional) {
        loggerPtr_->addLog(LogLevel::LOG_ERROR, "UniformInspectorUI::drawInput(InspectorReference)", "material not found!");
        value->modelSelection = 0;
        return false;
    }

    return changed;
}

bool UniformInspectorUI::drawRefInput_ObjectData(InspectorReference* value, Uniform* uniform) {
    if (value->referenceType != InspectorReferenceType::ObjectData) {
        loggerPtr_->addLog(LogLevel::LOG_ERROR, "UniformInspectorUI::drawRefInput_ObjectData", "Wrong reference type! not drawing this");
        return false;
    }

    std::optional<std::vector<std::string>> objectDataOptional = getObjectData(uniform->type);
    if (!objectDataOptional) {
        ImGui::TextDisabled("%s", "No object data available for this type!");
        return false;
    }

    value->initialized = false;
    bool changed = false;

    auto modelChoices = inspectorEngPtr_->getModelChoices();
    ImGui::TextDisabled("Source Object");
    ImGui::SetNextItemWidth(-1);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(styles_->framePadding.x, 0.0f));
    changed |= ImGui::Combo("##Source_model", &value->modelSelection, modelChoices.cstrings.data(), modelChoices.cstrings.size());
    ImGui::PopStyleVar();

    if (changed) {
        value->materialSelection = 0;
    }
    if (value->modelSelection == 0) {
        return changed;
    }

    value->referencedModelID = modelChoices.ids[value->modelSelection];

    std::vector<const char*> objectData = {""};
    for (int i = 0; i < objectDataOptional.value().size(); i++) {
        objectData.push_back(objectDataOptional.value()[i].c_str());
    }

    ImGui::TextDisabled("Source Value");
    ImGui::SetNextItemWidth(-1);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(styles_->framePadding.x, 0.0f));
    changed |= ImGui::Combo("##Source_value", &value->valueSelection, objectData.data(), objectData.size());
    ImGui::PopStyleVar();

    if (value->valueSelection > 0) {
        std::string referencedName = objectData[value->valueSelection];
        value->referencedValueName = referencedName;
        value->initialized = true;
    }

    return changed;
}
bool UniformInspectorUI::drawRefInput_SceneVar(InspectorReference* value, Uniform* uniform) {
    if (value->referenceType != InspectorReferenceType::SceneVariable) {
        loggerPtr_->addLog(LogLevel::LOG_ERROR, "UniformInspectorUI::drawRefInput_SceneVar", "Wrong reference type! not drawing this");
        return false;
    }

    std::optional<std::vector<std::string>> sceneVarsOpt = getSceneVariables(uniform->type);
    if (!sceneVarsOpt) {
        ImGui::TextDisabled("%s", "No scene variables available for this type!");
        return false;
    }

    bool changed = false;

    std::vector<const char*> sceneVars{""};
    for (int i = 0; i < sceneVarsOpt.value().size(); i++) {
        sceneVars.push_back(sceneVarsOpt.value()[i].c_str());
    }

    ImGui::TextDisabled("Source Value");
    ImGui::SetNextItemWidth(-1);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(styles_->framePadding.x, 0.0f));
    changed |= ImGui::Combo("##Source_value", &value->valueSelection, sceneVars.data(), sceneVars.size());
    ImGui::PopStyleVar();

    if (value->valueSelection >= sceneVars.size()) {
        value->valueSelection = 0;
    }
    if (value->valueSelection > 0) {
        std::string referencedName = sceneVars[value->valueSelection];
        value->referencedValueName = referencedName;
        value->initialized = true;
    }

    return changed;
}
