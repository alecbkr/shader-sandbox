#include "EditorUI.hpp"

#include <iostream>

#include "core/EditorEngine.hpp"
#include <string>

#include "exponential.hpp"
#include "imgui.h"
#include "core/EventDispatcher.hpp"
#include "core/logging/Logger.hpp"

float EditorUI::targetWidth = 0.0f;
float EditorUI::targetHeight = 0.0f;
ImVec2 EditorUI::windowPos = ImVec2(0, 0);

void renderEditor(Editor* editor) {
    ImGuiTableFlags lineNumberFlags = ImGuiTableFlags_BordersInnerV;
    ImGuiInputTextFlags textBoxFlags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackEdit;

    if (ImGui::BeginTable("##LineNumContainer", 2, lineNumberFlags, ImVec2(-1, -1))) {
        ImGui::TableSetupColumn("##LineNumberColumn", ImGuiTableColumnFlags_WidthFixed, ((int)std::log10((float) editor->lineCount) + 1)*6);
        ImGui::TableSetupColumn("##EditorColumn", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 0.0f));
        for (int i = 1; i <= editor->lineCount; i++) {
            ImGui::Text(std::to_string(i).c_str());
        }
        ImGui::PopStyleVar();

        ImGui::TableSetColumnIndex(1);
        ImGui::InputTextMultiline(
            "##EditorTextBox",
            editor->inputTextBuffer,
            editor->bufferSize,
            ImVec2(-1, ImGui::GetContentRegionAvail().y + editor->lineCount*13),
            textBoxFlags,
            EditorEngine::EditorInputCallback,
            editor
        );

        ImGui::EndTable();
    }
}

void EditorUI::render() {
    float menuBarHeight = ImGui::GetFrameHeight();

    int displayWidth = ImGui::GetIO().DisplaySize.x;
    int displayHeight = ImGui::GetIO().DisplaySize.y - menuBarHeight;
    float width = (float)displayWidth * EditorUI::targetWidth;
    float height = (float)displayHeight * EditorUI::targetHeight;

    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(windowPos.x, windowPos.y + menuBarHeight), ImGuiCond_Once);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("Editor", nullptr, flags)) {
        ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyScroll;
        if (ImGui::BeginTabBar("EditorTabs", tabBarFlags)) {
            for (int i = 0; i < EditorEngine::editors.size(); i++) {
                std::string tabTitle = EditorEngine::editors[i]->fileName + "##" + std::to_string(i + 1);
                bool openTab = true;

                if (!EditorEngine::editors[i]->filePath.empty()) {
                    if (ImGui::BeginTabItem(tabTitle.c_str(), &openTab)) {
                        renderEditor(EditorEngine::editors[i]);
                        EditorEngine::activeEditor = i;

                        ImGui::EndTabItem();
                    }
                } else {
                    if (ImGui::BeginTabItem(("Untitled##" + std::to_string(i)).c_str(), &openTab)) {
                        ImGui::Text("Enter File Name:");
                        char buf[256] = "\0";
                        if (ImGui::InputText("##FileNameInput", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue) && buf[0] != '\0') {
                            std::string filePath = "../shaders/" + std::string(buf);
                            try {
                                EditorEngine::createFile(filePath);
                                EditorEngine::editors[i]->destroy();
                                EditorEngine::editors[i] = new Editor(2056, filePath, buf);

                            } catch (const std::filesystem::filesystem_error& e) {
                                Logger::addLog(LogLevel::ERROR, "EditorEngine::createFile", std::string("Filesystem error: ") + e.what());
                            }
                        }

                        EditorEngine::activeEditor = i;
                        ImGui::EndTabItem();
                    }
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