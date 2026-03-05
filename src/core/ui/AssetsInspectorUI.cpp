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

void drawDirectory(std::filesystem::directory_entry entry) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 185, 175, 255));
    if (ImGui::TreeNode((entry.path().stem().string() + "##" + entry.path().string()).c_str())) {
        ImGui::Dummy(ImVec2(0, 12)); // TODO: keep filling out the tree and then finish drawAsset
        ImGui::TreePop();
    }
    ImGui::PopStyleColor();
}

void drawAsset(std::filesystem::directory_entry entry) {

}

void AssetsInspectorUI::draw() {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(26, 27, 33, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    float window_padding = 12.0f;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(window_padding, window_padding));
    if (ImGui::BeginChild("AssetsContent", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding)) {
        float inner_padding = 1.0f;
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
                IM_COL32(31, 32, 42, 255),
                6.0f,
                ImDrawFlags_RoundCornersTop
            );

            ImGui::GetWindowDrawList()->AddRect(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                IM_COL32(45, 47, 63, 255),   // border color
                6.0f,
                ImDrawFlags_RoundCornersTop,
                1.0f                          // thickness
            );

            ImGui::SetCursorPosY(inner_padding + window_padding);
            ImGui::Dummy(ImVec2(6.0f, 0.0f));
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
                IM_COL32(28, 30, 38, 255),
                6.0f,
                ImDrawFlags_RoundCornersBottom
            );

            ImGui::GetWindowDrawList()->AddRect(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                IM_COL32(45, 47, 63, 255),   // border color
                6.0f,
                ImDrawFlags_RoundCornersBottom,
                1.0f                          // thickness
            );

            //drawDirectory(project->projectRoot / "assets");
            for (auto& entry : std::filesystem::directory_iterator(project->projectRoot / "assets")) {
                if (entry.is_directory()) drawDirectory(entry);
                else if (entry.is_regular_file()) drawAsset(entry);
            }

            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 185, 175, 255));

            if (ImGui::TreeNode("Models")) {
                ImGui::Dummy(ImVec2(0, 12));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 8.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(248, 248, 242, 255));
                ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(40, 42, 54, 255));
                if (ImGui::BeginChild("Backpack", ImVec2(0, (window_padding * 2 + ImGui::CalcTextSize("backpack.fbx").y)), ImGuiChildFlags_AlwaysUseWindowPadding)) {
                    ImVec2 p = ImGui::GetWindowPos();
                    ImVec2 s = ImGui::GetWindowSize();

                    ImGui::GetWindowDrawList()->AddRect(
                        p,
                        ImVec2(p.x + s.x, p.y + s.y),
                        IM_COL32(45, 47, 63, 255),   // border color
                        3.0f,
                        ImDrawFlags_RoundCornersAll,
                        1.0f                          // thickness
                    );

                    ImGui::Text("backpack.fbx");
                }
                ImGui::EndChild();

                if (ImGui::BeginChild("tree", ImVec2(0, (window_padding * 2 + ImGui::CalcTextSize("tree_model.obj").y)), ImGuiChildFlags_AlwaysUseWindowPadding)) {
                    ImVec2 p = ImGui::GetWindowPos();
                    ImVec2 s = ImGui::GetWindowSize();

                    ImGui::GetWindowDrawList()->AddRect(
                        p,
                        ImVec2(p.x + s.x, p.y + s.y),
                        IM_COL32(45, 47, 63, 255),   // border color
                        3.0f,
                        ImDrawFlags_RoundCornersAll,
                        1.0f                          // thickness
                    );

                    ImGui::Text("tree_model.obj");
                }
                ImGui::EndChild();
                
                ImGui::PopStyleColor(2);
                ImGui::PopStyleVar(2);
                ImGui::TreePop();
            }
            ImGui::PopStyleColor();
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
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