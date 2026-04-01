#include "core/ui/AssetsInspectorUI.hpp"

#include "core/ui/Fonts.hpp"
#include "application/SettingsStyles.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <nfd/nfd.hpp>
#include <filesystem>
#include <cstdio>

AssetsInspectorUI::AssetsInspectorUI(Fonts* fonts, Project* project, SettingsStyles* styles) : fonts(fonts), project(project), styles(styles) {}

void AssetsInspectorUI::beginRename(const std::string& id, const std::string& currentName) {
    renamingID = id;
    std::snprintf(renameBuf, sizeof(renameBuf), "%s", currentName.c_str());
    renameJustStarted = true;
}

bool AssetsInspectorUI::drawRenameField(const std::filesystem::directory_entry& entry) {
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    if (entry.is_directory()) {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, styles->assetsTreeBodyColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, styles->assetsTreeBodyColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, styles->assetsTreeBodyColor);
    } else {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, styles->assetsFileBackgroundColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, styles->assetsFileBackgroundColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, styles->assetsFileBackgroundColor);
    }

    if (renameJustStarted) ImGui::SetKeyboardFocusHere();

    const bool enter = ImGui::InputText(
        ("##" + entry.path().string()).c_str(),
        renameBuf,
        sizeof(renameBuf),
        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll
    );

    if (renameJustStarted && ImGui::IsItemActive()) renameJustStarted = false;

    const bool hovered = ImGui::IsItemHovered();

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    const bool deactivatedAfterEdit = ImGui::IsItemDeactivatedAfterEdit();

    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        renamingID = "";
        return false;
    }

    const bool clicked_elsewhere = (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right)) && !hovered;

    if ((enter || deactivatedAfterEdit || clicked_elsewhere) && renameBuf[0] != '\0') {
        std::string newName = renameBuf;
        std::filesystem::path newPath = entry.path().parent_path() / newName;
        pendingRenamePaths[0] = entry.path();
        pendingRenamePaths[1] = newPath;
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

void AssetsInspectorUI::handlePendingRenaming() {
    if (!pendingRenamePaths[0].empty()) {
        std::filesystem::rename(pendingRenamePaths[0], pendingRenamePaths[1]);
        pendingRenamePaths[0] = std::filesystem::path{};
        pendingRenamePaths[1] = std::filesystem::path{};
    }
}

void AssetsInspectorUI::importAsset(const std::filesystem::path& destination) {
    NFD::UniquePath outPath;

    nfdfilteritem_t filters[] = {
        { "Assets", "obj,fbx,gltf,glb,png,jpg,jpeg,tga,bmp,hdr" }
    };

    nfdresult_t result = NFD::OpenDialog(outPath, filters, 1);
    if (result != NFD_OKAY) return;

    std::filesystem::path chosen = outPath.get();
    std::filesystem::copy_file(chosen, destination / chosen.filename(), std::filesystem::copy_options::overwrite_existing);
}

void AssetsInspectorUI::drawDirectory(std::filesystem::directory_entry entry, float padding) {
    const bool renaming = renamingID == entry.path().string();
    
    std::string label = renaming ? "" : entry.path().stem().string();

    ImGui::PushStyleColor(ImGuiCol_Text, styles->assetsDirectoryTextColor);

    if (renaming) ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
    const bool open = ImGui::TreeNodeEx((label + "##" + entry.path().string()).c_str());
    if (renaming) ImGui::PopItemFlag();
    
    ImGui::PopStyleColor();

    if (!renaming && renamingID == "") {
        ImVec4 textColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        if (ImGui::BeginPopupContextItem(("##directory_menu" + entry.path().string()).c_str())) {
            if (ImGui::MenuItem("Import Asset")) importAsset(entry.path());
            if (ImGui::MenuItem("New Folder")) {
                std::filesystem::create_directory(entry.path() / "untitled");
                beginRename((entry.path() / "untitled").string(), "untitled");
            }
            if (ImGui::MenuItem("Rename")) beginRename(entry.path().string(), entry.path().filename().string());
            if (ImGui::MenuItem("Delete")) pendingDeleteID = entry.path().string();
            ImGui::EndPopup();
        }
        ImGui::PopStyleColor();
    } else if (renaming) {
        ImGui::SameLine();
        drawRenameField(entry);
    }

    if (open) {
        for (auto& entry : std::filesystem::directory_iterator(entry.path())) {
            if (entry.is_directory()) drawDirectory(entry, padding);
            else if (entry.is_regular_file()) drawAsset(entry, padding);
        }
        ImGui::TreePop();
    }
}

void AssetsInspectorUI::drawAsset(std::filesystem::directory_entry entry, float padding) {
    ImVec4 textColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, styles->assetsFileBackgroundColor);
    if (ImGui::BeginChild(entry.path().string().c_str(), ImVec2(0, (padding * 2 + ImGui::CalcTextSize(entry.path().filename().string().c_str()).y)), ImGuiChildFlags_AlwaysUseWindowPadding)) {
        ImVec2 p = ImGui::GetWindowPos();
        ImVec2 s = ImGui::GetWindowSize();

        ImGui::GetWindowDrawList()->AddRect(
            p,
            ImVec2(p.x + s.x, p.y + s.y),
            ImGui::ColorConvertFloat4ToU32(styles->assetsBorderColor),
            3.0f,
            ImDrawFlags_RoundCornersAll,
            styles->assetsBorderThickness
        );

        if (renamingID == entry.path().string()) drawRenameField(entry);
        else ImGui::Text("%s", entry.path().filename().string().c_str());

        if (renamingID == "") {
            if (ImGui::BeginPopupContextWindow("##asset_ctx", ImGuiPopupFlags_MouseButtonRight)) {
                if (ImGui::MenuItem("Rename")) beginRename(entry.path().string(), entry.path().filename().string());
                if (ImGui::MenuItem("Delete")) pendingDeleteID = entry.path().string();
                ImGui::EndPopup();
            }
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleColor(2);
}

void AssetsInspectorUI::draw() {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, styles->assetsTabBackgroundColor);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    float window_padding = styles->assetsBodyPadding;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(window_padding, window_padding));
    if (ImGui::BeginChild("AssetsContent", ImVec2(0, 0), 
                          ImGuiChildFlags_AlwaysUseWindowPadding)) {
        float inner_padding = styles->assetsTitleInnerPadding;
        ImGui::PushFont(fonts->getL4());
        float directory_height = window_padding * 2 + inner_padding * 2 + ImGui::CalcTextSize("Assets").y;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0,0,0,0));
        if (ImGui::BeginChild("AssetsTitle", ImVec2(0, directory_height), 
                              ImGuiChildFlags_AlwaysUseWindowPadding)) {
            ImVec2 p = ImGui::GetWindowPos();
            ImVec2 s = ImGui::GetWindowSize();

            ImGui::GetWindowDrawList()->AddRectFilled(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles->assetsTitleBackgroundColor),
                styles->assetsBodyRounding,
                ImDrawFlags_RoundCornersTop
            );

            ImGui::GetWindowDrawList()->AddRect(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles->assetsBorderColor),
                styles->assetsBodyRounding,
                ImDrawFlags_RoundCornersTop,
                styles->assetsBorderThickness
            );

            ImGui::SetCursorPosY(inner_padding + window_padding);
            ImGui::Dummy(ImVec2(styles->assetsTitleOffset, 0.0f));
            ImGui::SameLine();
            ImGui::TextUnformatted("Assets");
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopFont();

        if (ImGui::BeginChild("AssetsTree", ImVec2(0, 0), 
                              ImGuiChildFlags_AlwaysUseWindowPadding)) {
            ImVec2 p = ImGui::GetWindowPos();
            ImVec2 s = ImGui::GetWindowSize();

            ImGui::GetWindowDrawList()->AddRectFilled(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles->assetsTreeBodyColor),
                styles->assetsBodyRounding,
                ImDrawFlags_RoundCornersBottom
            );

            ImGui::GetWindowDrawList()->AddRect(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles->assetsBorderColor),
                styles->assetsBodyRounding,
                ImDrawFlags_RoundCornersBottom,
                styles->assetsBorderThickness
            );

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 8.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);

            auto rootEntry = std::filesystem::directory_entry(project->projectRoot / "assets");
            if (ImGui::BeginPopupContextWindow("##root_ctx", ImGuiPopupFlags_MouseButtonRight)) {
                if (ImGui::MenuItem("Import Asset")) importAsset(rootEntry);
                if (ImGui::MenuItem("New Folder")) {
                    std::filesystem::create_directory(rootEntry.path() / "untitled");
                    beginRename((rootEntry.path() / "untitled").string(), "untitled");
                }
                ImGui::EndPopup();
            }
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
    handlePendingRenaming();
}
