#include "EditorUI.hpp"

#include <iostream>

#include "core/EditorEngine.hpp"
#include <string>

#include "exponential.hpp"
#include "imgui.h"


EditorUI::EditorUI(unsigned int bufferSize, std::string filePath, std::string fileName) {
    this->inputTextBuffer = new char[bufferSize];
    std::cout << fileName << std::endl;
    this->filePath = filePath;
    this->fileName = fileName;
    strcpy(this->inputTextBuffer, EditorEngine::getFileContents(filePath).c_str());
    this->bufferSize = bufferSize;

    this->lineCount = 1;

    int i = 0;
    while (this->inputTextBuffer[i] != '\0') {
        if (this->inputTextBuffer[i] == '\n') this->lineCount++;
        i++;
    }

    this->previousTextLen = i;
}

void EditorUI::render() {
    ImGuiTableFlags lineNumberFlags = ImGuiTableFlags_BordersInnerV;
    ImGuiInputTextFlags textBoxFlags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackEdit;

    if (ImGui::BeginTable("##LineNumContainer", 2, lineNumberFlags, ImVec2(-1, -1))) {
        ImGui::TableSetupColumn("##LineNumberColumn", ImGuiTableColumnFlags_WidthFixed, ((int)std::log10((float) this->lineCount) + 1)*6);
        ImGui::TableSetupColumn("##EditorColumn", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 0.0f));
        for (int i = 1; i <= this->lineCount; i++) {
            ImGui::Text(std::to_string(i).c_str());
        }
        ImGui::PopStyleVar();

        ImGui::TableSetColumnIndex(1);
        ImGui::InputTextMultiline(
            "##EditorTextBox",
            this->inputTextBuffer,
            this->bufferSize,
            ImVec2(-1, ImGui::GetContentRegionAvail().y + this->lineCount*13),
            textBoxFlags,
            EditorEngine::EditorInputCallback,
            this
        );

        ImGui::EndTable();
    }
}

void EditorUI::destroy() {
    free(this->inputTextBuffer);
    delete this;
}