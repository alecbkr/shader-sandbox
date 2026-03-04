#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <types.hpp>

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
    std::vector<std::unique_ptr<Directory>> childrenDirs;
    std::vector<Asset> childrenAssets;
};

struct Asset {
    u32 ID;
    std::string name;
    Directory* parent = nullptr;
    std::filesystem::path path;
    AssetType type = AssetType::AT_NOTYPE;
};

AssetType getAssetType(const std::filesystem::path& p);