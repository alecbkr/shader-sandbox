#include "core/ui/AssetsInspectorUI.hpp"

#include <algorithm>
#include <unordered_set>

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return (char)std::tolower(c); }
    );
    return s;
}

AssetType getAssetType(const std::filesystem::path& p) {
    const std::string ext = toLower(p.extension().string());

    static const std::unordered_set<std::string> modelExt{
        ".gltf",".glb",".obj",".fbx",".dae",".ply",".stl"
    };
    static const std::unordered_set<std::string> texExt{
        ".png",".jpg",".jpeg",".tga",".bmp",".hdr"
    };

    if (modelExt.count(ext)) return AssetType::AT_MODEL;
    if (texExt.count(ext)) return AssetType::AT_TEXTURE;
    return AssetType::AT_NOTYPE;
}

std::filesystem::path makeUniqueDestPath(std::filesystem::path dest) {
    if (!std::filesystem::exists(dest)) return dest;

    std::filesystem::path dir = dest.parent_path();
    std::filesystem::path stem = dest.stem();
    std::filesystem::path ext = dest.extension();

    for (int i = 1; i < 10000; i++) {
        std::filesystem::path candidate = dir / std::filesystem::path(stem.string() + " (" + std::to_string(i) + ")" + ext.string());
        if (!std::filesystem::exists(candidate)) return candidate;
    }
    return dest;
}

std::filesystem::path copyAssetIntoProject(const std::filesystem::path& src, const std::filesystem::path& destDir) {
    if (!std::filesystem::exists(src) || !std::filesystem::is_regular_file(src)) return {};

    AssetType type = getAssetType(src);
    if (type == AssetType::AT_NOTYPE) return {};

    std::filesystem::create_directories(destDir);

    std::filesystem::path dest = destDir / src.filename();
    dest = makeUniqueDestPath(dest);

    std::error_code ec;
    std::filesystem::copy_file(src, dest, std::filesystem::copy_options::none, ec);
    if (ec) return {};

    return dest;
}