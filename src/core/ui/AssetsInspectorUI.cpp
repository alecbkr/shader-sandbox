#include "core/ui/AssetsInspectorUI.hpp"

#include "core/TextureRegistry.hpp"
#include "texture/Texture.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <nfd/nfd.hpp>
#include <filesystem>
#include <cstdio>

namespace fs = std::filesystem;

AssetsInspectorUI::AssetsInspectorUI(Project* project) : project(project) {}

void AssetsInspectorUI::BeginRename(u32 id, const std::string& currentName) {
    renamingID = id;
    std::snprintf(renameBuf, sizeof(renameBuf), "%s", currentName.c_str());
    renameJustStarted = true;
}

bool AssetsInspectorUI::DrawRenameField(u32 id, std::string& name, const fs::path& oldPath, fs::path newPath) {
    if (renameJustStarted) {
        ImGui::SetKeyboardFocusHere();
        renameJustStarted = false;
    }

    ImGui::SetNextItemWidth(180.0f);
    const std::string inputId = "##rename_" + std::to_string(id);

    const bool enter = ImGui::InputText(
        inputId.c_str(),
        renameBuf,
        sizeof(renameBuf),
        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll
    );

    const bool deactivatedAfterEdit = ImGui::IsItemDeactivatedAfterEdit();

    if (ImGui::IsItemActive() && ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        renamingID = 0;
        return false;
    }

    if (enter || deactivatedAfterEdit) {
        if (renameBuf[0] != '\0') name = renameBuf;

        fs::rename(oldPath, newPath);
        renamingID = 0;
        return true;
    }

    return false;
}

void AssetsInspectorUI::AssetRow(Asset& asset) {
    ImGui::PushID((int)asset.ID);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 8));

    const float rowH = 44.0f;
    ImGui::BeginChild("##assetnode", ImVec2(0, rowH), true, ImGuiWindowFlags_NoScrollbar);

    // Right-click context (on the row child window)
    if (ImGui::BeginPopupContextWindow("##asset_ctx", ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("Rename")) BeginRename(asset.ID, asset.name);

        if (ImGui::MenuItem("Delete")) {
            pendingDeleteAssetID = asset.ID;
            pendingDeleteAsset   = &asset;
        }

        ImGui::EndPopup();
    }

    const bool isRenaming = (renamingID == asset.ID);

    if (!isRenaming) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
        ImGui::TextUnformatted(asset.name.c_str());
    } else {
        const fs::path newPath = asset.path.parent_path() / (asset.name + asset.path.extension().string());
        const bool renamed = DrawRenameField(asset.ID, asset.name, asset.path, newPath);
        if (renamed) asset.path = newPath;
    }

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopID();
}

void AssetsInspectorUI::DirectoryRow(Directory& dir) {
    ImGui::PushID((int)dir.ID);

    const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
    const bool open = ImGui::TreeNodeEx("##node", flags, "%s", "");

    ImGui::SameLine();

    const bool isRenaming = (renamingID == dir.ID);

    if (!isRenaming) {
        ImGui::TextUnformatted(dir.name.c_str());
    } else {
        const fs::path newPath = dir.path.parent_path() / dir.name;
        const bool renamed = DrawRenameField(dir.ID, dir.name, dir.path, newPath);
        if (renamed) dir.path = newPath;
    }

    // Context menu attached to the tree item
    if (ImGui::BeginPopupContextItem("##dir_ctx")) {
        if (ImGui::MenuItem("New Folder")) addDirectory(dir);
        if (ImGui::MenuItem("Rename")) BeginRename(dir.ID, dir.name);
        if (ImGui::MenuItem("Import Asset")) addAsset(dir);
        if (ImGui::MenuItem("Delete")) {
            pendingDeleteDirID = dir.ID;
            pendingDeleteDir   = &dir;
        }
        ImGui::EndPopup();
    }

    if (open) {
        for (auto& childDir : dir.childrenDirs) DirectoryRow(*childDir);
        for (auto& asset : dir.childrenAssets) AssetRow(asset);

        ImGui::TreePop();
    }

    ImGui::PopID();
}

void AssetsInspectorUI::HandlePendingDeletes() {
    if (pendingDeleteDirID != 0 && pendingDeleteDir != nullptr) {
        const fs::path removingPath = pendingDeleteDir->path;
        const bool removedFromTree = removeDirectory(*pendingDeleteDir);
        if (removedFromTree) fs::remove_all(removingPath);

        pendingDeleteDirID = 0;
        pendingDeleteDir   = nullptr;
    }

    if (pendingDeleteAssetID != 0 && pendingDeleteAsset != nullptr) {
        const fs::path removingPath = pendingDeleteAsset->path;
        const bool removedFromTree = removeAsset(*pendingDeleteAsset);
        if (removedFromTree) fs::remove(removingPath);

        pendingDeleteAssetID = 0;
        pendingDeleteAsset   = nullptr;
    }
}

void AssetsInspectorUI::draw() {
    if (ImGui::BeginChild("AssetsContent", ImVec2(0, 0))) {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        ImVec2 contentSize(10, avail.y);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 105, 180, 255));
        if (ImGui::BeginChild("Test", ImVec2(0, 0))) {
            ImGui::Text("This child has a pink background.");
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(100, 240, 180, 255));
        if (ImGui::BeginChild("Test2", ImVec2(0, 0))) {
            ImGui::Text("This child has a pink background.");
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }
    ImGui::EndChild();
    // const float bottomBarH = 44.0f;
    // const float importBtnW = 120.0f;
    // const float importBtnH = 32.0f;

    // ImVec2 avail = ImGui::GetContentRegionAvail();
    // ImVec2 contentSize(avail.x, std::max(0.0f, avail.y - bottomBarH));

    // ImGui::BeginChild("AssetsContent", contentSize, false, ImGuiWindowFlags_NoScrollbar);

    // // Root context menu (right click empty space)
    // if (ImGui::BeginPopupContextWindow("##assets_ctx", ImGuiPopupFlags_MouseButtonRight)) {
    //     if (ImGui::MenuItem("New Folder")) addDirectory(*root);
    //     if (ImGui::MenuItem("Import Asset")) addAsset(*root);
    //     ImGui::EndPopup();
    // }

    // for (auto& dir : root->childrenDirs) DirectoryRow(*dir);
    // for (auto& asset : root->childrenAssets) AssetRow(asset);

    // // Mutations happen once, after drawing the lists
    // HandlePendingDeletes();

    // ImGui::EndChild(); // AssetsContent

    // // ---------- Bottom bar ----------
    // ImGui::Separator();
    // ImGui::BeginChild("AssetsBottomBar", ImVec2(0, bottomBarH), false, ImGuiWindowFlags_NoScrollbar);

    // ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));
    // ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 8));

    // static char searchBuf[256] = "";
    // const float spacing = ImGui::GetStyle().ItemSpacing.x;

    // const float fullW = ImGui::GetContentRegionAvail().x;
    // const float searchW = std::max(0.0f, fullW - importBtnW - spacing);

    // ImGui::SetNextItemWidth(searchW);
    // ImGui::InputTextWithHint("##AssetsSearch", "Search...", searchBuf, sizeof(searchBuf));

    // ImGui::SameLine();

    // ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1);
    // if (ImGui::Button("+ Import", ImVec2(importBtnW, importBtnH))) addAsset(*root);

    // ImGui::PopStyleVar(2);
    // ImGui::EndChild(); // AssetsBottomBar
}

void AssetsInspectorUI::addAsset(Directory& dir) {
    std::vector<std::string> projectRelativePath = buildRelativePath(&dir);
    NFD::UniquePath outPath;

    nfdresult_t result = NFD::OpenDialog(outPath, nullptr, 0);
    if (result != NFD_OKAY) return;

    fs::path chosen = outPath.get();

    fs::path assetDest = project->projectRoot;
    for (auto& p : projectRelativePath) assetDest /= p;

    fs::path copiedTo = copyAssetIntoProject(chosen, assetDest);

    dir.childrenAssets.emplace_back(Asset{ idCount, copiedTo.stem().string(), &dir });
    idCount++;
}

bool AssetsInspectorUI::removeAsset(Asset& _asset) {
    Directory& parent = *_asset.parent;

    auto it = std::find_if(parent.childrenAssets.begin(), parent.childrenAssets.end(),
        [&_asset](const Asset& a) { return a.ID == _asset.ID; });

    if (it == parent.childrenAssets.end()) return false;

    parent.childrenAssets.erase(it);
    return true;
}

void AssetsInspectorUI::addDirectory(Directory& parent) {
    parent.childrenDirs.emplace_back(std::make_unique<Directory>(
        Directory{ idCount, "Untitled", &parent }));
    idCount++;
}

bool AssetsInspectorUI::removeDirectory(Directory& _dir) {
    Directory& parent = *_dir.parent;

    auto it = std::find_if(parent.childrenDirs.begin(), parent.childrenDirs.end(),
        [&_dir](const std::unique_ptr<Directory>& d) { return d->ID == _dir.ID; });

    if (it == parent.childrenDirs.end()) return false;

    Directory& dir = *(*it);

    if (!dir.childrenDirs.empty() || !dir.childrenAssets.empty()) return false;
    if (dir.ID == root->ID) return false;

    parent.childrenDirs.erase(it);
    return true;
}

std::vector<std::string> AssetsInspectorUI::buildRelativePath(Directory* dir) {
    std::vector<std::string> out;

    for (Directory* node = dir; node != nullptr; node = node->parent)
        out.emplace_back(node->name);

    std::reverse(out.begin(), out.end());
    return out;
}