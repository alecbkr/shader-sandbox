#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <string>
#include <filesystem>
#include <vector>
#include <types.hpp>

#include "application/Project.hpp"
#include "core/ui/AssetsManager.hpp"

class Fonts;

class AssetsInspectorUI {
public:
    AssetsInspectorUI() = delete;
    AssetsInspectorUI(Fonts* fonts, Project* project);
    void draw();

private:
    void BeginRename(u32 id, const std::string& currentName);
    bool DrawRenameField(u32 id, std::string& name, const std::filesystem::path& oldPath, std::filesystem::path newPath);
    void AssetRow(Asset& asset);
    void DirectoryRow(Directory& dir);
    void HandlePendingDeletes();

    u32 renamingID = 0;
    char renameBuf[256] = {};
    bool renameJustStarted = false;
    u32 pendingDeleteDirID = 0;
    Directory* pendingDeleteDir = nullptr;
    u32 pendingDeleteAssetID = 0;
    Asset* pendingDeleteAsset = nullptr;

    u32 idCount = 1;
    void addAsset(Directory& parent);
    bool removeAsset(Asset& asset);
    void addDirectory(Directory& parent);
    bool removeDirectory(Directory& dir);
    Directory* root = nullptr;
    std::vector<std::string> buildRelativePath(Directory* dir);
    Fonts* fonts;
    Project* project;
};

std::filesystem::path copyAssetIntoProject(const std::filesystem::path& src, const std::filesystem::path& destDir);