#include "application/Project.hpp"

#include <filesystem>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <types.hpp>

static u32 idCounter = 1;

void buildDirectory(const std::filesystem::path& path, Directory& dir) {
    std::filesystem::directory_entry entry(path);
    if (entry.is_regular_file()) {
        // test to see if its a proper asset file
        AssetType type = getAssetType(entry.path());
        if (type == AssetType::AT_NOTYPE) return;
        // turn it into an asset
        Asset asset = Asset(idCounter, entry.path().stem().string(), &dir, entry.path(), type);
        idCounter++;
        // put the asset into the Directory "dir"
        dir.childrenAssets.push_back(asset);
    }

    if (entry.is_directory()) {
        for (const auto& child : std::filesystem::directory_iterator(path)) {
            if (child.is_directory()) {
                dir.childrenDirs.emplace_back(std::make_unique<Directory>(
                    Directory(idCounter, child.path().stem().string(), &dir, child.path())));
                idCounter++;
                buildDirectory(dir.childrenDirs.back()->path, *dir.childrenDirs.back());
            } else if (child.is_regular_file()) buildDirectory(child.path(), dir);
        }
    }
}

Directory Project::buildAssetsDirectory() {
    Directory root = Directory(0, "assets", nullptr, projectRoot / "assets");
    buildDirectory(root.path, root);
    return root;
}