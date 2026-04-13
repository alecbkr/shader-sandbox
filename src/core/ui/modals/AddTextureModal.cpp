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
        if (ImGui::BeginTabBar("TextureTypeTabs")) {

            if (ImGui::BeginTabItem("Texture2D")) {
                if (type != AddedType::Texture2D) {
                    selectedFace = 0;
                    selectedPaths.fill(std::filesystem::path());
                    type = AddedType::Texture2D;
                }
                drawTexture2DPage();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Cubemap")) {
                if (type != AddedType::Cubemap) {
                    selectedFace = 0;
                    selectedPaths.fill(std::filesystem::path());
                    type = AddedType::Cubemap;
                }
                drawCubemapPage();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::EndChild();

    ImGui::Separator();
    if (ImGui::Button("Cancel")) {
        selectedFace = 0;
        selectedPaths.fill(std::filesystem::path());
        targetMaterial = nullptr;
        type = AddedType::Texture2D;
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Add")) {
        if (addTexture()) {
            selectedFace = 0;
            selectedPaths.fill(std::filesystem::path());
            targetMaterial = nullptr;
            type = AddedType::Texture2D;
            ImGui::CloseCurrentPopup();
        }
    }
}

void AddTextureModal::drawTexture2DPage() {
    ImGui::Spacing();

    

    std::filesystem::path selectedPath = drawTextureExplorer();
    if (!selectedPath.empty()) {
        selectedPaths[0] = selectedPath;
    }
}

void AddTextureModal::drawCubemapPage() {
    ImGui::Spacing();

    std::vector<std::string> selectedTextures(6, std::string(""));
    const char* faces[6] = {
        "Right", 
        "Left",
        "Top",
        "Bottom",
        "Front",
        "Back"
    };

    ImGuiTableFlags flags =
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_BordersOuter |
        ImGuiTableFlags_SizingStretchProp |
        ImGuiTableFlags_ScrollY;

    if (ImGui::BeginChild("CubemapTableContainer", ImVec2(0, 170), false)) {
        if (ImGui::BeginTable("##ImportedTexturesTable", 3, flags)) { 
            ImGui::TableSetupColumn("Face", ImGuiTableColumnFlags_WidthFixed, 140.0f);
            ImGui::TableSetupColumn("Texture Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableHeadersRow();

            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.7f, 0.15f, 0.15f, 1.0f));

            for (int i = 0; i < 6; i++) {
                ImGui::TableNextRow();

                bool isSelected = selectedFace == i;

                ImGui::TableNextColumn();
                if (ImGui::Selectable(faces[i], isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                    selectedFace = i;
                }
                
                ImGui::TableNextColumn();
                if (!selectedPaths[i].empty()) {
                    ImGui::TextUnformatted(selectedPaths[i].filename().string().c_str());
                }

                ImGui::TableNextColumn();
                if (!selectedPaths[i].empty()) {
                    ImGui::TextUnformatted(selectedPaths[i].extension().string().c_str());
                }
            }

            ImGui::PopStyleColor(3);
            ImGui::EndTable();
        }
        ImGui::EndChild();
    }
    std::filesystem::path result = drawTextureExplorer();
    if (!result.empty() && selectedFace < 6) {
        selectedPaths[selectedFace] = result;
        selectedFace++;
    }
}

bool AddTextureModal::addTexture() {
    if (type == AddedType::Texture2D && !selectedPaths[0].empty()) {
        unsigned int textureID = texCachePtr->createTexture2D(selectedPaths[0].string().c_str());
        targetMaterial->addTexture(textureID);
        return true;
    }
    else if (type == AddedType::Cubemap) {
        std::vector<std::string> cubemap_paths(6);
        bool hasAllTextures = true;
        for (int idx = 0; idx < 6; idx++) {
            if (selectedPaths[idx].empty()) {
                hasAllTextures = false;
                break;
            }
            else {
                cubemap_paths[idx] = selectedPaths[idx].string();
            }
        }

        if (hasAllTextures) {
            unsigned int cubemapID = texCachePtr->createCubeMap(cubemap_paths);
            targetMaterial->addTexture(cubemapID);
            return true;
        }
    }
    return false;
}

std::filesystem::path AddTextureModal::drawTextureExplorer() {
    std::filesystem::path selectedPath = "";
    ImGui::Spacing();

    if (ImGui::BeginChild("##TextureList", ImVec2(0, 0), false)) {
        if (!targetMaterial) {
            ImGui::TextDisabled("No material selected.");
            ImGui::EndChild();
            return "";
        }

        if (assetsDirPath.empty() || !std::filesystem::exists(assetsDirPath)) {
            ImGui::TextDisabled("Assets directory not found.");
            ImGui::EndChild();
            return "";
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

                selectedPath = drawDirectoryNode(assetsDirPath);

                ImGui::EndTable();
            }
        }
    }

    ImGui::EndChild();
    return selectedPath;
}

std::filesystem::path AddTextureModal::drawDirectoryNode(const std::filesystem::path& dirPath) {
    std::vector<std::filesystem::path> directories;
    std::vector<std::filesystem::path> validFiles;
    std::filesystem::path selectedPath = "";

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
            
            std::filesystem::path result = drawDirectoryNode(subdir);
            ImGui::TreePop();

            if (selectedPath.empty()) {
                selectedPath = result;
            }
        }
    }

    for (const auto& file : validFiles) {
        std::string filename = file.filename().string();
        std::string fileExt = file.extension().string();

        if (!fileExt.empty() && fileExt[0] == '.') fileExt = fileExt.substr(1);
        std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::toupper);

        std::string filePath = file.string();

        selectedPath = drawAssetTableRow(filename, fileExt, [this, filePath]() -> std::filesystem::path {
            if (!targetMaterial || !texCachePtr) return "";

            // unsigned int textureID = texCachePtr->createTexture2D(filePath.c_str());
            // targetMaterial->addTexture(textureID);

            // targetMaterial = nullptr;
            // ImGui::CloseCurrentPopup();

            return filePath;
        });
        if (!selectedPath.empty()) break;
    }
    return selectedPath;
}

bool AddTextureModal::isValidFileExtension(const std::filesystem::directory_entry& entry) {
    std::string ext = entry.path().extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return supportedTextureExtensions.contains(ext);
}

std::filesystem::path AddTextureModal::drawAssetTableRow(const std::string& name, const std::string& type, std::function<std::filesystem::path()> onClick) {
    std::filesystem::path selectedPath = "";
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    ImGuiSelectableFlags flags = ImGuiSelectableFlags_SpanAllColumns;
    if (ImGui::Selectable(name.c_str(), false, flags)) {
        selectedPath = onClick();
    }

    ImGui::TableNextColumn();

    float textWidth = ImGui::CalcTextSize(type.c_str()).x;
    float availableSpace = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availableSpace - textWidth);

    ImGui::TextDisabled("%s", type.c_str());

    return selectedPath;
}