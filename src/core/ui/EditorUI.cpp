#include "EditorUI.hpp"

#include "core/EditorEngine.hpp"
#include <string>

#include "exponential.hpp"
#include "imgui.h"

void renderEditor(Editor* editor) {
    ImGuiTableFlags lineNumberFlags = ImGuiTableFlags_BordersInnerV;
    ImGuiInputTextFlags textBoxFlags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackEdit;

    if (ImGui::BeginTable("##LineNumContainer", 2, lineNumberFlags, ImVec2(-1, -1))) {
        ImGui::TableSetupColumn("##LineNumberColumn", ImGuiTableColumnFlags_WidthFixed, ((int)std::log10((float) editor->lineCount) + 1)*6);
        ImGui::TableSetupColumn("##EditorColumn", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);
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
    ImGui::SetNextWindowSize(ImVec2( 500, 500), ImGuiCond_Once);

    if (ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_MenuBar)) {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New")) {
                    EditorEngine::spawnEditor(1024);
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if (ImGui::BeginTabBar("EditorTabs")) {
            for (int i = 0; i < EditorEngine::editors.size(); i++) {
                std::string tabTitle = "File " + std::to_string(i + 1);
                bool openTab = true;
                if (ImGui::BeginTabItem(tabTitle.c_str(), &openTab)) {

                    renderEditor(EditorEngine::editors[i]);

                    ImGui::EndTabItem();
                }

                if (!openTab) {
                    EditorEngine::editors[i]->destroy();
                    EditorEngine::editors.erase(EditorEngine::editors.begin() + i);
                }
            }

            ImGui::EndTabBar();
        }

    }
    ImGui::End();
}