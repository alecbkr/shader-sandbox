#include "DeleteProjectModal.hpp"

#include <filesystem>
#include <imgui/imgui.h>

#include "core/logging/Logger.hpp"
#include "persistence/ProjectLoader.hpp"
#include "persistence/ProjectSwitch.h"

bool DeleteProjectModal::initialize(Project* project, ProjectSwitch* projectSwitch, Logger* _loggerPtr) {
    if (initialized) return false;
    projectPtr = project;
    projectSwitchPtr = projectSwitch;
    loggerPtr = _loggerPtr;
    return true;
}

void DeleteProjectModal::draw() {
    ImGui::SetNextItemWidth(-1);
    ImGui::BeginChild("##open_project_root", ImVec2(500, 250), false);

    float footerHeight = ImGui::GetFrameHeightWithSpacing() + ImGui::GetStyle().ItemSpacing.y;
    ImGui::BeginChild("##project_list", ImVec2(0, -footerHeight), false);

    for (const auto& dirEntry : std::filesystem::directory_iterator(projectPtr->projectRoot / "..")) {
        ImGui::AlignTextToFramePadding();
        ImGui::Bullet();
        std::string fileName = dirEntry.path().filename().string();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", fileName.c_str());
        ImGui::SameLine();

        if (pendingDeletion != fileName) {
            if (ImGui::Button(("Delete##" + fileName).c_str())) {
                pendingDeletion = fileName;
            }
        } else {
            ImGui::SameLine();
            if (ImGui::Button("Confirm")) {
                if (dirEntry.path().filename() == projectPtr->projectTitle) {
                    *projectSwitchPtr = DELETE_CURRENT_PROJECT;
                } else {
                    try {
                        std::filesystem::remove_all(dirEntry.path());
                    } catch (const std::exception& e) {
                        loggerPtr->addLog(LogLevel::WARNING, "Delete Project", "Failed to delete " + fileName, e.what());
                    }
                }
                pendingDeletion = "";
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button(("Cancel##" + fileName).c_str())) {
                pendingDeletion = "";
            }
        }
    }

    ImGui::EndChild();

    if (ImGui::Button("Exit", ImVec2(-FLT_MIN, 0))) {
        pendingDeletion = "";
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndChild();
}