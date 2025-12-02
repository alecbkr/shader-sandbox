#include "Editor.hpp"

Editor::Editor(unsigned int bufferSize, unsigned int width, unsigned int height) {
    this->inputTextBuffer = new char[bufferSize];
    this->inputTextBuffer[0] = '\0';
    this->bufferSize = bufferSize;
    this->width = width;
    this->height = height;
}

void Editor::render() {
    ImGui::SetNextWindowSize(ImVec2((float) this->width, (float) this->height), ImGuiCond_Once);
    ImGui::Begin("Editor");

    ImGui::InputTextMultiline(
        "##EditorTextBox",
        this->inputTextBuffer,
        this->bufferSize,
        ImVec2(-1, -1),
        ImGuiInputTextFlags_AllowTabInput
    );

    ImGui::End();
}

void Editor::destroy() {
    delete[] this->inputTextBuffer;
}