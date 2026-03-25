#include "EditorUI.hpp"

#include <iostream>

#include "core/EditorEngine.hpp"
#include <string>

#include "imgui.h"
#include "components/SearchText.hpp"
#include "core/EventDispatcher.hpp"
#include "core/input/ContextManager.hpp"
#include "core/logging/Logger.hpp"

void EditorUI::render() {
    if (!contextManagerPtr->isEditor()) ImGui::SetWindowFocus(nullptr);

    float menuBarHeight = ImGui::GetFrameHeight();

    float displayWidth = ImGui::GetIO().DisplaySize.x;
    float displayHeight = ImGui::GetIO().DisplaySize.y - menuBarHeight;
    float width = displayWidth * targetWidth;
    float height = displayHeight * targetHeight;

    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(windowPos.x, windowPos.y + menuBarHeight), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;

    if (ImGui::Begin("Editor", nullptr, flags)) {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File##editor")) {
                if (ImGui::MenuItem("Clone Shader File")) {
                    //eventDispatcherPtr->TriggerEvent(CloneFileEvent());
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit##editor")) {
                if (ImGui::MenuItem("Find")) {
                    findBar = !findBar;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyScroll;
        if (ImGui::BeginTabBar("EditorTabs", tabBarFlags)) {
            projectPtr->openShaderFiles.clear();
            for (int i = 0; i < static_cast<int>(editorEngPtr->editors.size()); i++) {
                if (!editorEngPtr->editors[i]->readOnly) projectPtr->openShaderFiles.emplace_back(editorEngPtr->editors[i]->fileName);

                std::string tabTitle = editorEngPtr->editors[i]->fileName + "##" + std::to_string(i + 1);
                bool openTab = true;

                if (ImGui::BeginTabItem(tabTitle.c_str(), &openTab)) {
                    if (findBar) {
                        editorEngPtr->editors[i]->searcher.drawSearchUI([&](const SearchText::Match& match, const char* replace) {
                            editorEngPtr->editors[i]->textEditor.ReplaceMatch(match, replace);
                        });
                    }
                    editorEngPtr->editors[i]->render();
                    editorEngPtr->activeEditor = i;

                    ImGui::EndTabItem();
                }

                if (!openTab) {
                    editorEngPtr->editors[i]->destroy();
                    editorEngPtr->editors.erase(editorEngPtr->editors.begin() + i);

                    if (editorEngPtr->editors.empty()) editorEngPtr->activeEditor = -1;
                    i--;
                }
            }

            ImGui::EndTabBar();
        }

    }
    ImGui::End();
}

EditorUI::EditorUI() {
    initialized = false;
    loggerPtr = nullptr;
    editorEngPtr = nullptr;
    targetWidth = 0.0f;
    targetHeight = 0.0f;
    windowPos = ImVec2(0, 0);
    findBar = false;
}

#define TARGET_WIDTH 0.4f
#define TARGET_HEIGHT 0.7f
#define START_X 0;
#define START_Y 0;

bool EditorUI::initialize(Logger* _loggerPtr, EditorEngine* _editorEngPtr, ContextManager* _contextManagerPtr, EventDispatcher* _eventDispatcherPtr, Project* _projectPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Editor UI Initialization", "Editor UI was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    editorEngPtr = _editorEngPtr;
    contextManagerPtr = _contextManagerPtr;
    eventDispatcherPtr = _eventDispatcherPtr;
    projectPtr = _projectPtr;

    targetWidth = TARGET_WIDTH;
    targetHeight = TARGET_HEIGHT;
    windowPos.x = START_X;
    windowPos.y = START_Y;

    initialized = true;
    return true;
}