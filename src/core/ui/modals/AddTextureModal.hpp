#pragma once

#include "core/ui/modals/IModal.hpp"
#include "imgui.h"
#include <string>
#include <unordered_set>
#include <filesystem>
#include <functional>

class TextureCache;
class Material;

static const std::unordered_set<std::string> supportedTextureExtensions = {
    ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".hdr"
};

class AddTextureModal final : public IModal {
public:
    AddTextureModal() = default;

    bool initialize(TextureCache* _texCachePtr, const std::filesystem::path& _assetsDirPath);

    static constexpr const char* ID = "Add Texture";
    std::string_view id() const override { return ID; }

    void draw() override;

    void setTargetMaterial(Material* _targetMaterial);

private:
    TextureCache* texCachePtr = nullptr;
    Material* targetMaterial = nullptr;
    std::filesystem::path assetsDirPath;

    bool initialized = false;

    void drawTexturePage();
    void drawDirectoryNode(const std::filesystem::path& dirPath);
    bool isValidFileExtension(const std::filesystem::directory_entry& entry);
    void drawAssetTableRow(const std::string& name, const std::string& type, std::function<void()> onClick);
};