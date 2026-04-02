#include "core/ui/modals/AddTextureModal.hpp"
#include "texture/TextureCache.hpp"
#include "object/Material.hpp"

#include <filesystem>
#include <algorithm>
#include <vector>

bool AddTextureModal::initialize(TextureCache* _texCachePtr, const std::filesystem::path& _assetsDirPath) {
    if (initialized) return false;

    texCachePtr = _texCachePtr;
    assetsDirPath = _assetsDirPath;

    initialized = true;
    return true;
}

void AddTextureModal::setTargetMaterial(Material* _targetMaterial) {
    targetMaterial = _targetMaterial;
}

void AddTextureModal::draw() {
    if (!initialized) return;

    if (ImGui::BeginChild("AddTextureModalRoot", ImVec2(600, 400), false)) {
        drawTexturePage();
    }
    ImGui::EndChild();

    ImGui::Separator();
    if (ImGui::Button("Cancel")) {
        targetMaterial = nullptr;
        ImGui::CloseCurrentPopup();
    }
}

void AddTextureModal::drawTexturePage() {
    ImGui::Spacing();

    if (ImGui::BeginChild("##TextureList", ImVec2(0, 0), true)) {
        if (!targetMaterial) {
            ImGui::TextDisabled("No material selected.");
            ImGui::EndChild();
            return;
        }

        if (assetsDirPath.empty() || !std::filesystem::exists(assetsDirPath)) {
            ImGui::TextDisabled("Assets directory not found.");
            ImGui::EndChild();
            return;
        }

        bool foundTextures = false;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsDirPath)) {
            if (entry.is_regular_file() && isValidFileExtension(entry)) {
                foundTextures = true;
                break;
            }
        }

        if (!foundTextures) {
            ImGui::TextDisabled("No texture files (.png, .jpg, .jpeg, .bmp, .tga, .hdr) found in the project assets folder.");
        } else {
            ImGuiTableFlags flags =
                ImGuiTableFlags_RowBg |
                ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_SizingStretchProp |
                ImGuiTableFlags_ScrollY;

            if (ImGui::BeginTable("##ImportedTexturesTable", 2, flags)) {
                ImGui::TableSetupColumn("Texture Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80.0f);

                ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                ImGui::TableNextColumn();
                ImGui::TextDisabled("Texture Name");

                ImGui::TableNextColumn();
                const std::string typeLabel = "Type";
                float headerWidth = ImGui::CalcTextSize(typeLabel.c_str()).x;
                float availableSpace = ImGui::GetContentRegionAvail().x;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availableSpace - headerWidth);
                ImGui::TextDisabled("%s", typeLabel.c_str());

                drawDirectoryNode(assetsDirPath);

                ImGui::EndTable();
            }
        }
    }

    ImGui::EndChild();
}

void AddTextureModal::drawDirectoryNode(const std::filesystem::path& dirPath) {
    std::vector<std::filesystem::path> directories;
    std::vector<std::filesystem::path> validFiles;

    for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
        if (entry.is_directory()) {
            directories.push_back(entry.path());
        } else if (entry.is_regular_file() && isValidFileExtension(entry)) {
            validFiles.push_back(entry.path());
        }
    }

    for (const auto& subdir : directories) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        std::string folderName = subdir.filename().string();
        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_SpanAllColumns;
        bool isNodeOpen = ImGui::TreeNodeEx(folderName.c_str(), nodeFlags);

        ImGui::TableNextColumn();

        const std::string folderLabel = "Folder";
        float textWidth = ImGui::CalcTextSize(folderLabel.c_str()).x;
        float availableSpace = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availableSpace - textWidth);
        ImGui::TextDisabled("%s", folderLabel.c_str());

        if (isNodeOpen) {
            drawDirectoryNode(subdir);
            ImGui::TreePop();
        }
    }

    for (const auto& file : validFiles) {
        std::string filename = file.filename().string();
        std::string fileExt = file.extension().string();

        if (!fileExt.empty() && fileExt[0] == '.') fileExt = fileExt.substr(1);
        std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::toupper);

        std::string filePath = file.string();

        drawAssetTableRow(filename, fileExt, [this, filePath]() {
            if (!targetMaterial || !texCachePtr) return;

            unsigned int textureID = texCachePtr->createTexture2D(filePath.c_str());
            targetMaterial->addTexture(textureID);

            targetMaterial = nullptr;
            ImGui::CloseCurrentPopup();
        });
    }
}

bool AddTextureModal::isValidFileExtension(const std::filesystem::directory_entry& entry) {
    std::string ext = entry.path().extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return supportedTextureExtensions.contains(ext);
}

void AddTextureModal::drawAssetTableRow(const std::string& name, const std::string& type, std::function<void()> onClick) {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    ImGuiSelectableFlags flags = ImGuiSelectableFlags_SpanAllColumns;
    if (ImGui::Selectable(name.c_str(), false, flags)) {
        onClick();
    }

    ImGui::TableNextColumn();

    float textWidth = ImGui::CalcTextSize(type.c_str()).x;
    float availableSpace = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availableSpace - textWidth);

    ImGui::TextDisabled("%s", type.c_str());
}