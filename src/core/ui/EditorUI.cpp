#include "EditorUI.hpp"

#include <iostream>

#include "core/EditorEngine.hpp"
#include <string>

#include "imgui.h"
#include "components/SearchText.hpp"
#include "core/EventDispatcher.hpp"
#include "core/input/ContextManager.hpp"
#include "core/logging/Logger.hpp"

float EditorUI::targetWidth = 0.0f;
float EditorUI::targetHeight = 0.0f;
ImVec2 EditorUI::windowPos = ImVec2(0, 0);
bool EditorUI::findBar = false;
SearchText EditorUI::searcher;

void renderEditor(Editor* editor) {
    EditorUI::searcher.setSearchFlag(SearchUIFlags::ADVANCED | SearchUIFlags::REPLACE);

    if (EditorUI::searcher.GetisDirty()) {
        EditorUI::searcher.updateMatches(editor->textEditor.GetTextLines(), [&](const std::string &funcText) -> std::string {
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
    if (!ContextManager::isEditor()) ImGui::SetWindowFocus(nullptr);

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
            for (int i = 0; i < EditorEngine::editors.size(); i++) {
                std::string tabTitle = EditorEngine::editors[i]->fileName + "##" + std::to_string(i + 1);
                bool openTab = true;

                if (ImGui::BeginTabItem(tabTitle.c_str(), &openTab)) {
                    if (findBar) {
                        searcher.drawSearchUI();
                    }
                    renderEditor(EditorEngine::editors[i]);
                    EditorEngine::activeEditor = i;

                    ImGui::EndTabItem();
                }

                if (!openTab) {
                    EditorEngine::editors[i]->destroy();
                    EditorEngine::editors.erase(EditorEngine::editors.begin() + i);

                    if (EditorEngine::editors.empty()) EditorEngine::activeEditor = -1;
                    i--;
                }
            }

            ImGui::EndTabBar();
        }

    }
    ImGui::End();
}

#define TARGET_WIDTH 0.4f
#define TARGET_HEIGHT 0.7f
#define START_X 0;
#define START_Y 0;

bool EditorUI::initialize() {
    EditorUI::targetWidth = TARGET_WIDTH;
    EditorUI::targetHeight = TARGET_HEIGHT;
    EditorUI::windowPos.x = START_X;
    EditorUI::windowPos.y = START_Y;
    return true;
}