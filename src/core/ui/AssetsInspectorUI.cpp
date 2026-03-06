#include "core/ui/AssetsInspectorUI.hpp"

#include "core/ui/Fonts.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <nfd/nfd.hpp>
#include <filesystem>
#include <cstdio>

AssetsInspectorUI::AssetsInspectorUI(Fonts* fonts, Project* project) : fonts(fonts), project(project) {}

void AssetsInspectorUI::beginRename(const std::string& id, const std::string& currentName) {
    renamingID = id;
    std::snprintf(renameBuf, sizeof(renameBuf), "%s", currentName.c_str());
    renameJustStarted = true;
}

bool AssetsInspectorUI::drawRenameField(const std::filesystem::directory_entry& entry) {
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    if (entry.is_directory()) {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(31, 32, 42, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(31, 32, 42, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(31, 32, 42, 255));
    } else {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(40, 42, 54, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(40, 42, 54, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(40, 42, 54, 255));
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

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 185, 175, 255));// TODO: add this color to settings // assets directory text

    if (renaming) ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
    const bool open = ImGui::TreeNodeEx((label + "##" + entry.path().string()).c_str());
    if (renaming) ImGui::PopItemFlag();

    if (!renaming && renamingID == "") {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(248, 248, 242, 255));
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

        if (renamingID == entry.path().string()) drawRenameField(entry);
        else ImGui::Text(entry.path().filename().string().c_str());

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