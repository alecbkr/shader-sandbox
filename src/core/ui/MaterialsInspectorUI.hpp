#pragma once

#include "application/Project.hpp"

#include <vector>
#include <string>
#include <filesystem>
#include <limits>

class Fonts;
struct SettingsStyles;
class MaterialCache;
class TextureCache;
class ShaderRegistry;
class Material;

class MaterialsInspectorUI {
public:
    MaterialsInspectorUI() = delete;
    MaterialsInspectorUI(Fonts* fonts, SettingsStyles* styles, MaterialCache* matCache, TextureCache* texCache, ShaderRegistry* shaderReg, std::filesystem::path assetsDirPath);
    void draw();

private:
    bool drawRenameField(Material* mat);
    void handlePendingRename();
    void handlePendingDelete();

    Fonts* fonts;
    SettingsStyles* styles;
    MaterialCache* matCache;
    TextureCache* texCache;
    ShaderRegistry* shaderReg;
    std::filesystem::path assetsDirPath;

    std::vector<std::string> selectedPrograms;
    unsigned int renamingID = std::numeric_limits<unsigned int>::max();
    bool renameJustStarted = false;
    bool currRenaming = false;
    char renameBuf[256] = {};
    std::string pendingRename;
    Material* pendingRenameMat;
    Material* pendingDeleteMat = nullptr;
};