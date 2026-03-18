#include "SaveAsModal.hpp"

#include <imgui/imgui.h>

bool SaveAsModal::initialize(Logger* logger, Project* project, EventDispatcher* events, AppSettings* settings, bool* projectSwitch) {
    if (initialized) return false;
    loggerPtr = logger;
    projectPtr = project;
    eventPtr = events;
    settingsPtr = settings;
    projectSwitchPtr = projectSwitch;
    return true;
}

void SaveAsModal::draw() {
    if (projectPtr->previouslySaved) ImGui::TextUnformatted("This will create a copy of the existing project and open it in a new window.");

    ImGui::TextUnformatted("Project Name: ");
    ImGui::SameLine();
    ImGui::InputText("##saveAsFileName", inputBuffer, IM_ARRAYSIZE(inputBuffer));

    if (ImGui::Button("Save")) {
        if (!std::string(inputBuffer).empty()) {
            if (projectPtr->previouslySaved) {
                eventPtr->TriggerEvent({ EventType::SaveProject, false, std::monostate{} });

                std::filesystem::path oldRootPath = projectPtr->projectRoot;
                std::string fileName = findNextFileNumber(inputBuffer);
                std::filesystem::path newRootPath = projectPtr->projectRoot.parent_path() / fileName;
                projectPtr->projectRoot = newRootPath;
                projectPtr->projectTitle = fileName;
                projectPtr->projectShadersDir = newRootPath / "shaders";
                projectPtr->projectJSON = newRootPath / "project.json";

                std::filesystem::create_directories(projectPtr->projectRoot);
                std::filesystem::create_directories(projectPtr->projectShadersDir);
                for (auto& dirEntry : std::filesystem::directory_iterator(oldRootPath / "shaders")) {
                    std::filesystem::copy(dirEntry.path(), projectPtr->projectShadersDir / dirEntry.path().filename());
                }
                settingsPtr->projectToOpen = fileName;
                *projectSwitchPtr = true;
            } else {
                projectPtr->previouslySaved = true;

                std::filesystem::path oldRootPath = projectPtr->projectRoot;
                std::string fileName = findNextFileNumber(inputBuffer);
                std::filesystem::path newRootPath = projectPtr->projectRoot.parent_path() / fileName;

                std::filesystem::rename(oldRootPath, newRootPath);
                projectPtr->projectRoot = newRootPath;
                projectPtr->projectTitle = fileName;
                projectPtr->projectShadersDir = newRootPath / "shaders";
                projectPtr->projectJSON = newRootPath / "project.json";

                eventPtr->TriggerEvent({ EventType::SaveProject, false, std::monostate{} });
            }

            ImGui::CloseCurrentPopup();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        ImGui::CloseCurrentPopup();
    }
}

std::string SaveAsModal::findNextFileNumber(const std::string& startingName) {
    int i = 1;
    std::string newName = startingName + "(" + std::to_string(i) + ")";

    if (!std::filesystem::exists(projectPtr->projectRoot.parent_path() / startingName)) {
        return startingName;
    }

    while (std::filesystem::exists(projectPtr->projectRoot.parent_path()/ newName)) {
        newName = startingName + "(" + std::to_string(i) + ")";
        i++;
    }

    return newName;
}
