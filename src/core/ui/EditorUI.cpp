#include "EditorUI.hpp"

#include <iostream>

#include "core/EditorEngine.hpp"
#include <string>

#include "imgui.h"
#include "components/SearchText.hpp"
#include "core/EventDispatcher.hpp"
#include "core/input/ContextManager.hpp"
#include "core/logging/Logger.hpp"
SearchText EditorUI::searcher;

void EditorUI::renderEditor(Editor* editor) {
    searcher.setSearchFlag(SearchUIFlags::ADVANCED | SearchUIFlags::REPLACE);

    if (searcher.GetisDirty()) {
        searcher.updateMatches(editor->textEditor.GetTextLines(), [&](const std::string &funcText) -> std::string {
            return funcText;
        });
    }

    editor->textEditor.Render("ShaderEditor");
}

void EditorUI::drawActiveFind(std::string activeLine, ImVec2 textPos) {
    const auto& activeMatch = searcher.getActiveMatch();

    if (activeMatch.charIdx + activeMatch.length <= activeLine.size()) {
        std::string textBefore = activeLine.substr(0, activeMatch.charIdx);
        std::string textMatch = activeLine.substr(activeMatch.charIdx, activeMatch.length);

        float offsetX = ImGui::CalcTextSize(textBefore.c_str()).x;
        float width = ImGui::CalcTextSize(textMatch.c_str()).x;

        ImGui::GetWindowDrawList()->AddRectFilled(
            ImVec2(textPos.x + offsetX, textPos.y),
            ImVec2(textPos.x + offsetX + width, textPos.y + ImGui::GetTextLineHeight()),
            IM_COL32(200, 200, 200, 100)
        );
    }

}

void EditorUI::render() {
    if (!contextManagerPtr->isEditor()) ImGui::SetWindowFocus(nullptr);

    float menuBarHeight = ImGui::GetFrameHeight();

    int displayWidth = ImGui::GetIO().DisplaySize.x;
    int displayHeight = ImGui::GetIO().DisplaySize.y - menuBarHeight;
    float width = (float)displayWidth * EditorUI::targetWidth;
    float height = (float)displayHeight * EditorUI::targetHeight;

    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(windowPos.x, windowPos.y + menuBarHeight), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;

    if (ImGui::Begin("Editor", nullptr, flags)) {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Find")) {
                    findBar = !findBar;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyScroll;
        if (ImGui::BeginTabBar("EditorTabs", tabBarFlags)) {
            for (int i = 0; i < editorEngPtr->editors.size(); i++) {
                std::string tabTitle = editorEngPtr->editors[i]->fileName + "##" + std::to_string(i + 1);
                bool openTab = true;

                if (ImGui::BeginTabItem(tabTitle.c_str(), &openTab)) {
                    if (findBar) {
                        searcher.drawSearchUI();
                    }
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
    findBar = false;
}

#define TARGET_WIDTH 0.4f
#define TARGET_HEIGHT 0.7f
#define START_X 0;
#define START_Y 0;

bool EditorUI::initialize(Logger* _loggerPtr, EditorEngine* _editorEngPtr, ContextManager* _contextManagerPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Editor UI Initialization", "Editor UI was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    editorEngPtr = _editorEngPtr;
    contextManagerPtr = _contextManagerPtr;

    targetWidth = TARGET_WIDTH;
    targetHeight = TARGET_HEIGHT;
    windowPos.x = START_X;
    windowPos.y = START_Y;

    initialized = true;
    return true;
}