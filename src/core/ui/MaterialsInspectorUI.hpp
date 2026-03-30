#pragma once

#include "application/Project.hpp"

#include <vector>
#include <string>
#include <filesystem>

class Fonts;
struct SettingsStyles;
class MaterialCache;
class TextureCache;
class ShaderRegistry;

class MaterialsInspectorUI {
public:
    MaterialsInspectorUI() = delete;
    MaterialsInspectorUI(Fonts* fonts, SettingsStyles* styles, MaterialCache* matCache, TextureCache* texCache, ShaderRegistry* shaderReg, std::filesystem::path assetsDirPath);
    void draw();

private:
    Fonts* fonts;
    SettingsStyles* styles;
    MaterialCache* matCache;
    TextureCache* texCache;
    ShaderRegistry* shaderReg;
    std::filesystem::path assetsDirPath;

    std::vector<std::string> selectedPrograms;
};