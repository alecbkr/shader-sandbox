#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <string>
#include <filesystem>
#include <vector>
#include <types.hpp>

class TextureRegistry;

enum AssetType {
    AT_NOTYPE,
    AT_MODEL,
    AT_TEXTURE
};

struct Asset;
struct Directory {
    u32 ID;
    std::string name;
    Directory* parent = nullptr;
    std::filesystem::path path;
    std::vector<Directory> childrenDirs;
    std::vector<Asset> childrenAssets;
};

struct Asset {
    u32 ID;
    std::string name;
    Directory* parent = nullptr;
    std::filesystem::path path;
    AssetType type = AssetType::AT_NOTYPE;
};

class AssetsInspectorUI {
public:
    AssetsInspectorUI() = delete;
    AssetsInspectorUI(std::filesystem::path projectRoot);
    void draw(TextureRegistry* textureRegPtr);

private:
    void AssetRow(Asset& asset);
    void DirectoryRow(Directory& dir);

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
    Directory root = Directory{0, "assets"};
    std::vector<std::string> buildRelativePath(Directory* dir);

    std::filesystem::path projectRoot;
};

std::filesystem::path copyAssetIntoProject(const std::filesystem::path& src, const std::filesystem::path& destDir);