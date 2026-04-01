#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <string>
#include <filesystem>
#include <vector>
#include <types.hpp>
#include <array>

#include "application/Project.hpp"

class Fonts;
struct SettingsStyles;

class AssetsInspectorUI {
public:
    AssetsInspectorUI() = delete;
    AssetsInspectorUI(Fonts* fonts, Project* project, SettingsStyles* styles);
    void draw();

private:
    void beginRename(const std::string& id, const std::string& currentName);
    bool drawRenameField(const std::filesystem::directory_entry& entry);
    void handlePendingDeletes();
    void handlePendingRenaming();
    void importAsset(const std::filesystem::path& destination);

    std::string renamingID = "";
    char renameBuf[256] = {};
    bool renameJustStarted = false;
    std::array<std::filesystem::path, 2> pendingRenamePaths;
    std::string pendingDeleteID = "";

    void drawDirectory(std::filesystem::directory_entry entry, float padding);
    void drawAsset(std::filesystem::directory_entry entry, float padding);
    Fonts* fonts;
    Project* project;
    SettingsStyles* styles;
};