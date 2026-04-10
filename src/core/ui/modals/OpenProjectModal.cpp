#include "OpenProjectModal.hpp"

#include <filesystem>
#include <imgui/imgui.h>

#include "persistence/ProjectLoader.hpp"
#include "persistence/ProjectSwitch.h"

bool OpenProjectModal::initialize(
    Project* project, AppSettings* settings, ModelCache* _modelCachePtr, MaterialCache* _materialCachePtr, ShaderRegistry* _shaderRegPtr, ProjectSwitch* projectSwitch) {
    if (initialized) return false;
    projectPtr = project;
    settingsPtr = settings;
    modelCachePtr = _modelCachePtr;
    materialCachePtr = _materialCachePtr;
    projectSwitchPtr = projectSwitch;
    shaderRegPtr = _shaderRegPtr;
    return true;
}

void OpenProjectModal::draw() {
    ImGui::SetNextItemWidth(-1);
    ImGui::BeginChild("##open_project_root", ImVec2(500, 250), false);

    float footerHeight = ImGui::GetFrameHeightWithSpacing() + ImGui::GetStyle().ItemSpacing.y;
    ImGui::BeginChild("##project_list", ImVec2(0, -footerHeight), false);

    for (const auto& dirEntry : std::filesystem::directory_iterator(projectPtr->projectRoot / "..")) {
        ImGui::Bullet();
        std::string fileName = dirEntry.path().filename().string();
        ImGui::Selectable(fileName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_DontClosePopups);

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
            ProjectLoader::save(*projectPtr, modelCachePtr, materialCachePtr, shaderRegPtr);
            settingsPtr->projectToOpen = fileName;
            *projectSwitchPtr = SWITCH;
            ImGui::CloseCurrentPopup();
        }
    }

    ImGui::EndChild();

    if (ImGui::Button("Exit", ImVec2(-FLT_MIN, 0))) {
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndChild();
}