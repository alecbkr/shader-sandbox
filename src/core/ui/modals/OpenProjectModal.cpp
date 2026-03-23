#include "OpenProjectModal.hpp"

#include <filesystem>
#include <imgui/imgui.h>

#include "persistence/ProjectLoader.hpp"

bool OpenProjectModal::initialize(Project* project, AppSettings* settings, bool* projectSwitch) {
    if (initialized) return false;
    projectPtr = project;
    settingsPtr = settings;
    projectSwitchPtr = projectSwitch;
    return true;
}

void OpenProjectModal::draw() {
    for (const auto& dirEntry : std::filesystem::directory_iterator(projectPtr->projectRoot / "..")) {
        ImGui::Bullet();
        std::string fileName = dirEntry.path().filename().string();
        ImGui::Selectable(fileName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_DontClosePopups);

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
            ProjectLoader::save(*projectPtr);
            settingsPtr->projectToOpen = fileName;
            *projectSwitchPtr = true;
            ImGui::CloseCurrentPopup();
        }
    }
}