#include "Editor.hpp"

#include <fstream>
#include <string>

#include "imgui.h"

std::string Editor::getFileContents(const char *filename) {
    std::ifstream in(filename, std::ios::binary);
    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    return "";
}

Editor::Editor(unsigned int bufferSize, unsigned int width, unsigned int height) {
    this->inputTextBuffer = new char[bufferSize];
    strcpy(this->inputTextBuffer, getFileContents("../shaders/default.frag").c_str());
    this->bufferSize = bufferSize;
    this->width = width;
    this->height = height;
}

static int EditorInputCallback(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
        bool nextLineTrigger = data->BufTextLen > 0 && data->Buf[data->CursorPos - 1] == '\n';
        bool previousOpenBrace = data->BufTextLen > 1 && data->Buf[data->CursorPos - 2] == '{';
        if (nextLineTrigger && previousOpenBrace) {
            const char* closeBrace = "\t\n}";
            data->InsertChars(data->CursorPos, closeBrace);

            data->CursorPos -= 2;
        }
    }

    return 0;
}

void Editor::render() {
    ImGui::SetNextWindowSize(ImVec2((float) this->width, (float) this->height), ImGuiCond_Once);

    ImGui::Begin("Editor");

    ImGuiTableFlags lineNumberFlags = ImGuiTableFlags_BordersInnerV;
    ImGuiInputTextFlags textBoxFlags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackEdit;

    ImGui::BeginTable("##LineNumContainer", 2, lineNumberFlags, ImVec2(-1, -1));
    ImGui::TableSetupColumn("##LineNumberColumn", ImGuiTableColumnFlags_WidthFixed, 10);
    ImGui::TableSetupColumn("##EditorColumn", ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    ImGui::BeginChild("##LineNumberChild", ImVec2(-1, -1), false, ImGuiWindowFlags_NoScrollbar);

    int totalLines = 9;
    for (int i = 1; i <= totalLines; i++) {
        ImGui::TextUnformatted(std::to_string(i).c_str());
    }

    ImGui::EndChild();

    ImGui::TableSetColumnIndex(1);
    ImGui::InputTextMultiline(
        "##EditorTextBox",
        this->inputTextBuffer,
        this->bufferSize,
        ImVec2(-1, -1),
        textBoxFlags,
        EditorInputCallback
    );

    ImGui::EndTable();

    ImGui::End();
}

void Editor::destroy() {
    free(this->inputTextBuffer);
}