#include "EditorUI.hpp"

#include <iostream>

#include "core/EditorEngine.hpp"
#include <string>

#include "exponential.hpp"
#include "imgui.h"
// #include "core/EventDispatcher.hpp"
#include "core/logging/Logger.hpp"

void renderEditor(Editor* editor) {
    editor->textEditor.Render("ShaderEditor");
}

void EditorUI::render() {
    float menuBarHeight = ImGui::GetFrameHeight();

    int displayWidth = ImGui::GetIO().DisplaySize.x;
    int displayHeight = ImGui::GetIO().DisplaySize.y - menuBarHeight;
    float width = (float)displayWidth * EditorUI::targetWidth;
    float height = (float)displayHeight * EditorUI::targetHeight;

    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(windowPos.x, windowPos.y + menuBarHeight), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("Editor", nullptr, flags)) {
        ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyScroll;
        if (ImGui::BeginTabBar("EditorTabs", tabBarFlags)) {
            for (int i = 0; i < editorEngPtr->editors.size(); i++) {
                std::string tabTitle = editorEngPtr->editors[i]->fileName + "##" + std::to_string(i + 1);
                bool openTab = true;

                if (ImGui::BeginTabItem(tabTitle.c_str(), &openTab)) {
                    renderEditor(editorEngPtr->editors[i]);
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
}

#define TARGET_WIDTH 0.4f
#define TARGET_HEIGHT 0.7f
#define START_X 0;
#define START_Y 0;

bool EditorUI::initialize(Logger* _loggerPtr, EditorEngine* _editorEngPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Editor UI Initialization", "Editor UI was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    editorEngPtr = _editorEngPtr;

    EditorUI::targetWidth = TARGET_WIDTH;
    EditorUI::targetHeight = TARGET_HEIGHT;
    EditorUI::windowPos.x = START_X;
    EditorUI::windowPos.y = START_Y;

    initialized = true;
    return true;
}