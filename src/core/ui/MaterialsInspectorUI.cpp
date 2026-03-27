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

MaterialsInspectorUI::MaterialsInspectorUI(Fonts* fonts, SettingsStyles* styles, MaterialCache* matCache, TextureCache* texCache, ShaderRegistry* shaderReg) : fonts(fonts), styles(styles), matCache(matCache), texCache(texCache), shaderReg(shaderReg) {}

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
            float buttonPaddingY = 0.0f;
            float buttonPaddingX = 6.0f;
            
            float buttonWidth = buttonPaddingX * 2 + plusSize.x;
            
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - buttonWidth);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(buttonPaddingX, buttonPaddingY));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
            ImGui::Button("+", ImVec2(buttonPaddingX * 2 + plusSize.x, buttonPaddingY * 2 + plusSize.y));
            ImGui::PopStyleVar(2);
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
                std::string label = mat->name + " (" + std::to_string(mat->ID) + ")" + "##" + std::to_string(mat->ID);
                bool matOpen = ImGui::CollapsingHeader(label.c_str());

                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Rename")) {
                        // start rename process
                    }

                    if (ImGui::MenuItem("Delete")) {
                        // prompt delete process
                    }

                    ImGui::EndPopup();
                }

                if (matOpen) {
                    const auto& programs = shaderReg->getPrograms();

                    if (ImGui::BeginCombo("Program", mat->getProgramID().c_str())) {
                        for (auto& [name, program] : programs) {
                            bool isSelected = (mat->getProgramID() == name);

                            if (ImGui::Selectable(name.c_str(), isSelected)) {
                                mat->setProgramID(name);
                            }

                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    if (ImGui::CollapsingHeader(("Textures##" + std::to_string(mat->ID)).c_str())) {
                        ImGui::Button("+");
                        for (auto& tex : mat->getAllTexturePaths(texCache)) {
                            ImGui::TextUnformatted(tex.c_str());
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
}