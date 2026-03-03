#include "core/ui/AssetsInspectorUI.hpp"

#include "core/TextureRegistry.hpp"
#include "texture/Texture.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <nfd/nfd.hpp>
#include <iostream>

AssetsInspectorUI::AssetsInspectorUI(std::filesystem::path _projectRoot) : projectRoot(_projectRoot) {};

// --- Small helper for a "list row" style item (icon + name + optional trash icon) ---
void AssetsInspectorUI::AssetRow(Asset& asset) {
    ImGui::PushID((int)asset.ID);
    // Row background
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 8));
    const float rowH = 44.0f;

    ImGui::BeginChild("##assetnode", ImVec2(0, rowH), true, ImGuiWindowFlags_NoScrollbar);

    if (ImGui::BeginPopupContextWindow("##dir_ctx", ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("Rename")) {
            renamingID = asset.ID;
            std::snprintf(renameBuf, sizeof(renameBuf), "%s", asset.name.c_str());
            renameJustStarted = true;
        };
        if (ImGui::MenuItem("Delete")) {
            pendingDeleteAssetID = asset.ID;
            pendingDeleteAsset = &asset;
        }
        ImGui::EndPopup();
    }

    const bool isRenaming = (renamingID == asset.ID);

    if (!isRenaming) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
        ImGui::TextUnformatted(asset.name.c_str());
    } else {
        if (renameJustStarted) {
            ImGui::SetKeyboardFocusHere();
            renameJustStarted = false;
        }

        ImGui::SetNextItemWidth(180.0f);

        std::string inputId = "##rename_" + std::to_string(asset.ID);
        bool enter = ImGui::InputText(inputId.c_str(), renameBuf, sizeof(renameBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
        bool deactivatedAfterEdit = ImGui::IsItemDeactivatedAfterEdit();

        if (ImGui::IsItemActive() && ImGui::IsKeyPressed(ImGuiKey_Escape)) renamingID = 0;
        else if (enter || deactivatedAfterEdit) {
            if (renameBuf[0] != '\0') asset.name = renameBuf;
            renamingID = 0;
        }
    }

    ImGui::EndChild();

    ImGui::PopStyleVar(2);
    ImGui::PopID();
}

void AssetsInspectorUI::DirectoryRow(Directory& dir) {
    ImGui::PushID((int)dir.ID);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;

    const bool open = ImGui::TreeNodeEx("##node", flags, "%s", "");

    ImGui::SameLine();

    const bool isRenaming = (renamingID == dir.ID);

    if (!isRenaming) ImGui::TextUnformatted(dir.name.c_str());
    else {
        if (renameJustStarted) {
            ImGui::SetKeyboardFocusHere();
            renameJustStarted = false;
        }

        ImGui::SetNextItemWidth(180.0f);

        std::string inputId = "##rename_" + std::to_string(dir.ID);
        bool enter = ImGui::InputText(inputId.c_str(), renameBuf, sizeof(renameBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
        bool deactivatedAfterEdit = ImGui::IsItemDeactivatedAfterEdit();

        if (ImGui::IsItemActive() && ImGui::IsKeyPressed(ImGuiKey_Escape)) renamingID = 0;
        else if (enter || deactivatedAfterEdit) {
            if (renameBuf[0] != '\0') dir.name = renameBuf;
            renamingID = 0;
        }
    }

    if (ImGui::BeginPopupContextItem("##dir_ctx")) {
        if (ImGui::MenuItem("New Folder")) addDirectory(dir);
        if (ImGui::MenuItem("Rename")) {
            renamingID = dir.ID;
            std::snprintf(renameBuf, sizeof(renameBuf), "%s", dir.name.c_str());
            renameJustStarted = true;
        };
        if (ImGui::MenuItem("Import Asset")) addAsset(dir);
        if (ImGui::MenuItem("Delete")) {
            pendingDeleteDirID = dir.ID;
            pendingDeleteDir = &dir;
        }
        ImGui::EndPopup();
    }

    if (open) {
        for (auto& dirChild : dir.childrenDirs) {
            DirectoryRow(dirChild);
        }
        for (auto& asset : dir.childrenAssets) {
            AssetRow(asset);
        }
        ImGui::TreePop();
    }
    
    ImGui::PopID();
}

void AssetsInspectorUI::draw(TextureRegistry* textureRegPtr) {
    // ---------- Layout constants ----------
    const float bottomBarH = 44.0f;
    const float importBtnW = 120.0f;
    const float importBtnH = 32.0f;

    // ---------- Main content (scroll area) ----------
    // Reserve space at bottom for the search/import bar
    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImVec2 contentSize(avail.x, std::max(0.0f, avail.y - bottomBarH));

    ImGui::BeginChild("AssetsContent", contentSize, false, ImGuiWindowFlags_NoScrollbar);
    
    if (ImGui::BeginPopupContextWindow("##assets_ctx", ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("New Folder")) addDirectory(root);
        if (ImGui::MenuItem("Import Asset")) addAsset(root);
        ImGui::EndPopup();
    }

    for (auto& dir : root.childrenDirs) DirectoryRow(dir);
    if (pendingDeleteDirID != 0) {
        removeDirectory(*pendingDeleteDir);
        pendingDeleteDirID = 0;
        pendingDeleteDir = nullptr;
    }

    for (auto& asset : root.childrenAssets) AssetRow(asset);
    if (pendingDeleteAssetID != 0) {
        removeAsset(*pendingDeleteAsset);
        pendingDeleteAssetID = 0;
        pendingDeleteAsset = nullptr;
    }

    ImGui::EndChild(); // AssetsContent

    // ---------- Bottom bar (Search + Import) ----------
    ImGui::Separator();
    ImGui::BeginChild("AssetsBottomBar", ImVec2(0, bottomBarH), false, ImGuiWindowFlags_NoScrollbar);

    // Make it feel like a toolbar
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 8));

    // Search input takes "most of the width"
    static char searchBuf[256] = "";
    const float spacing = ImGui::GetStyle().ItemSpacing.x;

    float fullW = ImGui::GetContentRegionAvail().x;
    float searchW = std::max(0.0f, fullW - importBtnW - spacing);

    ImGui::SetNextItemWidth(searchW);
    ImGui::InputTextWithHint("##AssetsSearch", "Search...", searchBuf, sizeof(searchBuf));

    ImGui::SameLine();

    // "+ Import" button on bottom right (no functionality)
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1); // tiny vertical nudge
    if (ImGui::Button("+ Import", ImVec2(importBtnW, importBtnH))) {
        addAsset(root);
    }

    ImGui::PopStyleVar(2);
    ImGui::EndChild(); // AssetsBottomBar
}

void AssetsInspectorUI::addAsset(Directory& dir) {
    std::vector<std::string> projectRelativePath = buildRelativePath(&dir);
    NFD::UniquePath outPath;

    nfdresult_t result = NFD::OpenDialog(outPath, nullptr, 0);

    if (result == NFD_OKAY) {
        std::filesystem::path chosen = outPath.get();
        std::filesystem::path assetDest = projectRoot;
        for (auto& path : projectRelativePath) {
            assetDest = assetDest / path;
        }
        std::filesystem::path copiedTo = copyAssetIntoProject(chosen, assetDest);
        dir.childrenAssets.emplace_back(Asset{ idCount, copiedTo.stem().string(), &dir });
        idCount++;
    }
}

bool AssetsInspectorUI::removeAsset(Asset& _asset) {
    Directory& parent = *_asset.parent;
    auto it = std::find_if(parent.childrenAssets.begin(), parent.childrenAssets.end(),
        [_asset](const Asset& asset) {
            return asset.ID == _asset.ID;
        }
    );

    if (it != parent.childrenAssets.end()) {
        parent.childrenAssets.erase(it);
        return true;
    }
    return false;
}

void AssetsInspectorUI::addDirectory(Directory& parent) {
    parent.childrenDirs.emplace_back(Directory{ idCount, "Untitled" , &parent });
    idCount++;
}

bool AssetsInspectorUI::removeDirectory(Directory& _dir) {
    Directory& parent = *_dir.parent;
    auto it = std::find_if(parent.childrenDirs.begin(), parent.childrenDirs.end(),
        [_dir](const Directory& dir) {
            return dir.ID == _dir.ID;
        }
    );

    if (it == parent.childrenDirs.end()) return false;

    Directory& dir = *it;

    if (!dir.childrenDirs.empty() || !dir.childrenAssets.empty()) return false;
    if (dir.ID == root.ID) return false;

    parent.childrenDirs.erase(it);
    return true;
}

std::vector<std::string> AssetsInspectorUI::buildRelativePath(Directory* dir) {
    std::vector<std::string> out;

    for (Directory* node = dir; node != nullptr; node = node->parent) {
        out.emplace_back(node->name);
    }

    std::reverse(out.begin(), out.end());
    return out;
}