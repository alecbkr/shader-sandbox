#include "core/ui/MaterialsInspectorUI.hpp"

#include "core/ui/Fonts.hpp"
#include "application/SettingsStyles.hpp"
#include "object/MaterialCache.hpp"
#include "texture/TextureCache.hpp"
#include "core/ShaderRegistry.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <string>
#include <vector>

MaterialsInspectorUI::MaterialsInspectorUI(Fonts* fonts, SettingsStyles* styles, MaterialCache* matCache, TextureCache* texCache, ShaderRegistry* shaderReg, std::filesystem::path assetsDirPath) : fonts(fonts), styles(styles), matCache(matCache), texCache(texCache), shaderReg(shaderReg), assetsDirPath(assetsDirPath) {}

bool MaterialsInspectorUI::drawRenameField(Material* mat) {
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    if (renameJustStarted) ImGui::SetKeyboardFocusHere();

    const bool enter = ImGui::InputText(
        ("##material" + std::to_string(mat->ID)).c_str(),
        renameBuf,
        sizeof(renameBuf),
        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll
    );

    if (renameJustStarted && ImGui::IsItemActive()) renameJustStarted = false;

    const bool hovered = ImGui::IsItemHovered();

    ImGui::PopStyleVar();

    const bool deactivateAfterEdit = ImGui::IsItemDeactivatedAfterEdit();

    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        renamingID = std::numeric_limits<unsigned int>::max();
        currRenaming = false;
        return false;
    }

    const bool clicked_elsewhere = (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right)) && !hovered;

    if ((enter || deactivateAfterEdit || clicked_elsewhere) && renameBuf[0] != '\0') {
        std::string newName = renameBuf;
        pendingRename = newName;
        pendingRenameMat = mat;
        renamingID = std::numeric_limits<unsigned int>::max();
        currRenaming = false;
        return true;
    }

    return false;
}

void MaterialsInspectorUI::handlePendingRename() {
    if (!pendingRename.empty()) {
        pendingRenameMat->setName(pendingRename);
        pendingRename = "";
        pendingRenameMat = nullptr;
    }
}

void MaterialsInspectorUI::handlePendingDelete() {
    if (pendingDeleteMat != nullptr) {
        matCache->deleteMaterial(pendingDeleteMat->ID);
        pendingDeleteMat = nullptr;
    }
}

void MaterialsInspectorUI::draw() {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, styles->materialsTabBackgroundColor);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);

    float window_padding = styles->materialsBodyPadding;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(window_padding, window_padding));

    if (ImGui::BeginChild("MaterialsContent", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding)) {
        ImGui::PushFont(fonts->getL4());

        float inner_padding = styles->materialsTitleInnerPadding;
        float directory_height = window_padding * 2 + inner_padding * 2 + ImGui::CalcTextSize("Materials").y;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0,0,0,0));

        if (ImGui::BeginChild("MaterialsTitle", ImVec2(0, directory_height), ImGuiChildFlags_AlwaysUseWindowPadding)) {
            ImVec2 p = ImGui::GetWindowPos();
            ImVec2 s = ImGui::GetWindowSize();

            ImGui::GetWindowDrawList()->AddRectFilled(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles->materialsTitleBackgroundColor),
                styles->materialsBodyRounding,
                ImDrawFlags_RoundCornersTop
            );

            ImGui::GetWindowDrawList()->AddRect(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles->materialsBorderColor),
                styles->materialsBodyRounding,
                ImDrawFlags_RoundCornersTop,
                styles->materialsBorderThickness
            );

            ImGui::SetCursorPosY(inner_padding + window_padding);
            ImGui::Dummy(ImVec2(styles->materialsTitleOffset, 0.0f));
            ImGui::SameLine();
            ImGui::TextUnformatted("Materials");
            
            ImVec2 plusSize = ImGui::CalcTextSize("+");
            float buttonPaddingX = 6.5f;
            
            float buttonWidth = plusSize.x + buttonPaddingX * 2;
            float buttonHeight = plusSize.y;
            
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - buttonWidth);

            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.6f, 0.6f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
            
            if (ImGui::Button("+##addmaterial", ImVec2(buttonWidth, buttonHeight))) {
                matCache->createBlankMaterial();
            }
            ImGui::PopStyleVar(3);
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::PopFont();

        if (ImGui::BeginChild("MaterialsContent", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding)) {
            ImVec2 p = ImGui::GetWindowPos();
            ImVec2 s = ImGui::GetWindowSize();

            ImGui::GetWindowDrawList()->AddRectFilled(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles->materialsTreeBodyColor),
                styles->materialsBodyRounding,
                ImDrawFlags_RoundCornersBottom
            );

            ImGui::GetWindowDrawList()->AddRect(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles->materialsBorderColor),
                styles->materialsBodyRounding,
                ImDrawFlags_RoundCornersBottom,
                styles->materialsBorderThickness
            );

            for (auto& mat : matCache->getAllMaterials()) {
                const bool renaming = renamingID == mat->ID;

                std::string label = renaming
                    ? ("##material_header_" + std::to_string(mat->ID))
                    : (mat->getName() + " (" + std::to_string(mat->ID) + ")##" + std::to_string(mat->ID));
                
                if (renaming) ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
                bool matOpen = ImGui::CollapsingHeader(label.c_str());
                if (renaming) ImGui::PopItemFlag();

                if (!renaming && !currRenaming) {
                    if (ImGui::BeginPopupContextItem()) {
                        if (ImGui::MenuItem("Rename")) {
                            renamingID = mat->ID;
                            renameJustStarted = true;
                            currRenaming = true;
                        }
                        
                        if (ImGui::MenuItem("Delete")) {
                            pendingDeleteMat = mat;
                        }
                        
                        ImGui::EndPopup();
                    }
                } else if (renaming) {
                    ImGui::SameLine();
                    drawRenameField(mat);
                }

                if (matOpen) {
                    const auto& programs = shaderReg->getPrograms();

                    int currentType = (int)mat->getMaterialType();
                    static const char* materialTypes[3] = {"Opaque", "Cutout", "Translucent"};

                    if (ImGui::Combo("Type", &currentType, materialTypes, 3)) {
                        matCache->changeMaterialType(mat->ID, (MaterialType)currentType);
                        
                    }

                    if (ImGui::BeginCombo("Program", mat->getProgramID().c_str())) {
                        for (auto& [name, program] : programs) {
                            bool isSelected = (mat->getProgramID() == name);

                            if (ImGui::Selectable(name.c_str(), isSelected)) {
                                matCache->changeMaterialProgram(mat->ID, name);
                                mat->setProgramID(name);
                            }

                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    if (ImGui::CollapsingHeader(("Textures##" + std::to_string(mat->ID)).c_str())) {
                        static char textureAddBuf[256] = "";

                        ImGui::InputText(("##TextureInput" + std::to_string(mat->ID)).c_str(),
                                        textureAddBuf, sizeof(textureAddBuf));
                        ImGui::SameLine();

                        if (ImGui::Button(("+##TextureInput" + std::to_string(mat->ID)).c_str())) {
                            unsigned int id = texCache->createTexture2D((assetsDirPath / textureAddBuf).string().c_str());
                            mat->addTexture(id);
                        }

                        auto textures = mat->getAllTexturePaths(texCache);

                        for (int i = 0; i < (int)textures.size(); i++) {
                            ImGui::PushID(i);

                            ImGui::TextUnformatted(textures[i].c_str());

                            if (ImGui::BeginPopupContextItem("TexturePopup")) {
                                if (ImGui::MenuItem("Remove")) {
                                }
                                ImGui::EndPopup();
                            }

                            ImGui::PopID();
                        }
                    }
                }
            }

        }
        ImGui::EndChild();
    }
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();

    handlePendingRename();
    handlePendingDelete();
}