#include "core/ui/MaterialsInspectorUI.hpp"

#include "core/ui/Fonts.hpp"
#include "application/SettingsStyles.hpp"
#include "object/MaterialCache.hpp"
#include "texture/TextureCache.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/ui/modals/ModalManager.hpp"
#include "core/ui/modals/AddTextureModal.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <string>
#include <vector>

MaterialsInspectorUI::MaterialsInspectorUI(Fonts* fonts, SettingsStyles* styles, MaterialCache* matCache, TextureCache* texCache, ShaderRegistry* shaderReg, ModalManager* modalManager, std::filesystem::path assetsDirPath) : fonts(fonts), styles(styles), matCache(matCache), texCache(texCache), shaderReg(shaderReg), assetsDirPath(assetsDirPath), modalManager(modalManager) {
    addTextureModal = dynamic_cast<AddTextureModal*>(
        modalManager->getModalPtr("Add Texture")
    );
}

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

std::string makeRelativeToAssets(const std::string& fullPath) {
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

            for (size_t i = 0; i < matCache->getAllMaterials().size(); ++i) {
                Material* mat = matCache->getAllMaterials()[i];
                const bool renaming = renamingID == mat->ID;

                std::string label = renaming
                    ? ("##material_header_" + std::to_string(mat->ID))
                    : (mat->getName() + "##" + std::to_string(mat->ID));

                ImGui::PushID(mat->ID);

                // Card styling
                ImGui::PushStyleColor(ImGuiCol_ChildBg, styles->materialsTitleBackgroundColor);
                ImGui::PushStyleColor(ImGuiCol_Border, styles->materialsBorderColor);
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, styles->materialsBodyRounding);
                ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, styles->materialsBorderThickness);

                // Header styling
                ImGui::PushStyleColor(ImGuiCol_Header, styles->materialsTitleBackgroundColor);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(
                    styles->materialsTitleBackgroundColor.x * 1.15f,
                    styles->materialsTitleBackgroundColor.y * 1.15f,
                    styles->materialsTitleBackgroundColor.z * 1.15f,
                    styles->materialsTitleBackgroundColor.w
                ));
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, styles->materialsTitleBackgroundColor);

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 6.0f));

                if (ImGui::BeginChild(("MaterialCard##" + std::to_string(mat->ID)).c_str(),
                                    ImVec2(0, 0),
                                    ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding)) {

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 4.0f));

                    if (renaming) ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
                    bool matOpen = ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth);
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
                        ImGui::PushStyleColor(ImGuiCol_Separator, styles->materialsBorderColor);
                        ImGui::Separator();
                        ImGui::PopStyleColor();
                        ImGui::Dummy(ImVec2(0.0f, 2.0f));
                        ImGui::Indent(8.0f);

                        const auto& programs = shaderReg->getPrograms();

                        int currentType = (int)mat->getMaterialType();
                        static const char* materialTypes[3] = {"Opaque", "Cutout", "Translucent"};

                        if (ImGui::Combo(("Type##" + std::to_string(mat->ID)).c_str(), &currentType, materialTypes, 3)) {
                            matCache->changeMaterialType(mat->ID, (MaterialType)currentType);
                            
                        }
                        ShaderProgram* prog = shaderReg->getProgram(mat->getProgramID());
                        std::string progName = prog ? prog->name : "no program";

                        bool noProgram = prog ? false : true;
                        if (noProgram) {
                            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
                        }

                        if (ImGui::BeginCombo(("Program##" + std::to_string(mat->ID)).c_str(), progName.c_str())) {
                            for (auto& [ID, program] : programs) {
                                bool isSelected = (mat->getProgramID() == ID);

                                if (ImGui::Selectable(program->name.c_str(), isSelected)) {
                                    matCache->changeMaterialProgram(mat->ID, ID);
                                    mat->setProgramID(ID);
                                }

                                if (isSelected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }

                        if (noProgram){
                            ImGui::PopStyleColor();
                        }

                        if (ImGui::CollapsingHeader(("Textures##" + std::to_string(mat->ID)).c_str())) {
                            if (ImGui::Button(("Add Texture##" + std::to_string(mat->ID)).c_str())) {
                                if (addTextureModal) {
                                    addTextureModal->setTargetMaterial(mat);
                                    modalManager->open("Add Texture");
                                }
                            }

                            auto textureData = mat->getAllTextureUnitsAndPaths(texCache);

                            int i = 0;
                            for (auto& [texUnit, path] : textureData) {
                                ImGui::PushID(i);

                                std::string relativePath = makeRelativeToAssets(path);

                                ImGui::TextUnformatted(("Texture Unit: " + std::to_string(texUnit) + " | " + relativePath).c_str());

                                if (ImGui::BeginPopupContextItem("TexturePopup")) {
                                    if (ImGui::MenuItem("Remove")) {
                                        // remove texture here when your material API supports it
                                    }
                                    ImGui::EndPopup();
                                }

                                ImGui::PopID();
                                i++;
                            }
                        }
                        ImGui::Unindent(8.0f);
                    }

                    ImGui::PopStyleVar(); // FramePadding
                }

                ImGui::EndChild();

                ImGui::PopStyleVar(3);   // WindowPadding, ChildBorderSize, ChildRounding
                ImGui::PopStyleColor(5); // ChildBg + 3 header colors
                ImGui::PopID();

                if (i + 1 < matCache->getAllMaterials().size()) {
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
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