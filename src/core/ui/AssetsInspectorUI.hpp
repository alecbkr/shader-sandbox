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
    void beginRename(const std::string& id, const std::string& currentName);
    bool drawRenameField(std::string id, std::string& name, const std::filesystem::path& oldPath, std::filesystem::path newPath);
    void handlePendingDeletes();

    std::string renamingID = "";
    char renameBuf[256] = {};
    bool renameJustStarted = false;
    std::string pendingDeleteID = "";

    void drawDirectory(std::filesystem::directory_entry entry, float padding);
    void drawAsset(std::filesystem::directory_entry entry, float padding);
    Fonts* fonts;
    Project* project;
};

std::filesystem::path copyAssetIntoProject(const std::filesystem::path& src, const std::filesystem::path& destDir);