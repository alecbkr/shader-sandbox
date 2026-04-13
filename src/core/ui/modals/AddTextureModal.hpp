#pragma once

#include "core/ui/modals/IModal.hpp"
#include "imgui.h"
#include <string>
#include <unordered_set>
#include <filesystem>
#include <functional>
#include <array>

class TextureCache;
class Material;

static const std::unordered_set<std::string> supportedTextureExtensions = {
    ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".hdr"
};

class AddTextureModal final : public IModal {
private: 
    enum class AddedType {
        Texture2D,
        Cubemap
    };

public:
    AddTextureModal() = default;

    bool initialize(TextureCache* _texCachePtr, const std::filesystem::path& _assetsDirPath);

    static constexpr const char* ID = "Add Texture";
    std::string_view id() const override { return ID; }

    void draw() override;

    void setTargetMaterial(Material* _targetMaterial);

private:
    AddedType type = AddedType::Texture2D;
    int selectedFace = 0;
    std::array<std::filesystem::path, 6> selectedPaths;
    TextureCache* texCachePtr = nullptr;
    Material* targetMaterial = nullptr;
    std::filesystem::path assetsDirPath;

    bool initialized = false;

    void drawTexture2DPage();
    void drawCubemapPage();
    bool addTexture();

    bool isValidFileExtension(const std::filesystem::directory_entry& entry);
    std::filesystem::path drawTextureExplorer();
    std::filesystem::path drawDirectoryNode(const std::filesystem::path& dirPath);
    std::filesystem::path drawAssetTableRow(const std::string& name, const std::string& type, std::function<std::filesystem::path()> onClick);
};