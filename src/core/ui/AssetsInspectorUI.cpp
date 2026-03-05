#include "core/ui/AssetsInspectorUI.hpp"

#include "core/ui/Fonts.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <nfd/nfd.hpp>
#include <filesystem>
#include <cstdio>

namespace fs = std::filesystem;

AssetsInspectorUI::AssetsInspectorUI(Fonts* fonts, Project* project) : fonts(fonts), project(project) {}

void AssetsInspectorUI::beginRename(const std::string& id, const std::string& currentName) {
    renamingID = id;
    std::snprintf(renameBuf, sizeof(renameBuf), "%s", currentName.c_str());
    renameJustStarted = true;
}

bool AssetsInspectorUI::drawRenameField(std::string id, std::string& name, const std::filesystem::path& oldPath, std::filesystem::path newPath) {
    if (renameJustStarted) {
        ImGui::SetKeyboardFocusHere();
        renameJustStarted = false;
    }

    ImGui::SetNextItemWidth(180.0f);
    const std::string inputId = "##rename_" + id;

    const bool enter = ImGui::InputText(
        inputId.c_str(),
        renameBuf,
        sizeof(renameBuf),
        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll
    );

    const bool deactivatedAfterEdit = ImGui::IsItemDeactivatedAfterEdit();

    if (ImGui::IsItemActive() && ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        renamingID = "";
        return false;
    }

    if (enter || deactivatedAfterEdit) {
        if (renameBuf[0] != '\0') name = renameBuf;

        fs::rename(oldPath, newPath);
        renamingID = "";
        return true;
    }

    return false;
}

void AssetsInspectorUI::handlePendingDeletes() {
    if (pendingDeleteID != "") {
        const std::filesystem::path removingPath(pendingDeleteID);
        std::filesystem::remove(removingPath);
        pendingDeleteID = "";
    }
}

void AssetsInspectorUI::drawDirectory(std::filesystem::directory_entry entry, float padding) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 185, 175, 255));
    if (ImGui::TreeNode((entry.path().stem().string() + "##" + entry.path().string()).c_str())) {
            for (auto& entry : std::filesystem::directory_iterator(entry.path())) {
                if (entry.is_directory()) drawDirectory(entry, padding);
                else if (entry.is_regular_file()) drawAsset(entry, padding);
            }
        ImGui::TreePop();
    }
    ImGui::PopStyleColor();
}

void AssetsInspectorUI::drawAsset(std::filesystem::directory_entry entry, float padding) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(248, 248, 242, 255)); // TODO: pull this color from already existing settings // normal text color
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(40, 42, 54, 255)); // TODO: add this color to settings // asset file background
    if (ImGui::BeginChild(entry.path().string().c_str(), ImVec2(0, (padding * 2 + ImGui::CalcTextSize(entry.path().filename().string().c_str()).y)), ImGuiChildFlags_AlwaysUseWindowPadding)) {
        ImVec2 p = ImGui::GetWindowPos();
        ImVec2 s = ImGui::GetWindowSize();

        ImGui::GetWindowDrawList()->AddRect(
            p,
            ImVec2(p.x + s.x, p.y + s.y),
            IM_COL32(45, 47, 63, 255),// TODO: add this color to settings   // assets border color
            3.0f,
            ImDrawFlags_RoundCornersAll,
            1.0f                      // TODO: add this value to settings    // assets border thickness
        );

        ImGui::Text(entry.path().filename().string().c_str());

        if (ImGui::BeginPopupContextWindow("##asset_ctx", ImGuiPopupFlags_MouseButtonRight)) {
            if (ImGui::MenuItem("Rename")) beginRename(entry.path().string(), entry.path().filename().string());
            if (ImGui::MenuItem("Delete")) pendingDeleteID = entry.path().string();
            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleColor(2);
}

void AssetsInspectorUI::draw() {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(26, 27, 33, 255)); // TODO: add this color to settings // assets background
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    float window_padding = 12.0f; // TODO: add this value to settings // assets body padding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(window_padding, window_padding));
    if (ImGui::BeginChild("AssetsContent", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding)) {
        float inner_padding = 1.0f; // TODO: add this value to settings // assets title inner padding
        ImGui::PushFont(fonts->getL4());
        float directory_height = window_padding * 2 + inner_padding * 2 + ImGui::CalcTextSize("Assets").y;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0,0,0,0));
        if (ImGui::BeginChild("AssetsTitle", ImVec2(0, directory_height), ImGuiChildFlags_AlwaysUseWindowPadding)) {
            ImVec2 p = ImGui::GetWindowPos();
            ImVec2 s = ImGui::GetWindowSize();

            ImGui::GetWindowDrawList()->AddRectFilled(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                IM_COL32(31, 32, 42, 255), // TODO: add this color to settings // assets title background
                6.0f, // TODO: add this value to settings // assets body rounding
                ImDrawFlags_RoundCornersTop
            );

            ImGui::GetWindowDrawList()->AddRect(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                IM_COL32(45, 47, 63, 255), // TODO: add this color to settings   // assets body border color
                6.0f, // TODO: add this value to settings // assets body rounding
                ImDrawFlags_RoundCornersTop,
                1.0f                       // TODO: add this value to settings   // assets body border thickness
            );

            ImGui::SetCursorPosY(inner_padding + window_padding);
            ImGui::Dummy(ImVec2(6.0f, 0.0f)); // TODO: add this value to settings // assets title offset
            ImGui::SameLine();
            ImGui::TextUnformatted("Assets");
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopFont();

        if (ImGui::BeginChild("AssetsTree", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding)) {
            ImVec2 p = ImGui::GetWindowPos();
            ImVec2 s = ImGui::GetWindowSize();

            ImGui::GetWindowDrawList()->AddRectFilled(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                IM_COL32(28, 30, 38, 255), // TODO: add this color to settings // tree body color
                6.0f, // TODO: add this value to settings // assets body rounding
                ImDrawFlags_RoundCornersBottom
            );

            ImGui::GetWindowDrawList()->AddRect(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                IM_COL32(45, 47, 63, 255), // TODO: add this color to settings   // assets body border color
                6.0f, // TODO: add this value to settings // assets body rounding
                ImDrawFlags_RoundCornersBottom,
                1.0f                       // TODO: add this value to settings   // assets body border thickness
            );

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 8.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
            for (auto& entry : std::filesystem::directory_iterator(project->projectRoot / "assets")) {
                if (entry.is_directory()) drawDirectory(entry, window_padding);
                else if (entry.is_regular_file()) drawAsset(entry, window_padding);
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

    handlePendingDeletes();
}