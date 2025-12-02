#include "Editor.hpp"

Editor::Editor(unsigned int bufferSize, unsigned int width, unsigned int height) {
    this->inputTextBuffer = new char[bufferSize];
    this->inputTextBuffer[0] = '\0';
    this->bufferSize = bufferSize;
    this->width = width;
    this->height = height;
    this->uniformColor[0] = 1.0f;
    this->uniformColor[1] = 1.0f;
    this->uniformColor[2] = 1.0f;
    this->inspectorWidth = 350;
    this->inspectorHeight = 100;
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

    ImGui::SetNextWindowSize(ImVec2((float) this->inspectorWidth, (float) this->inspectorHeight), ImGuiCond_Once);
    ImGui::Begin("Inspector");
    ImGui::Text("Uniform Values");
    ImGui::InputFloat3("inColor (vec3)", this->uniformColor);

    ImGui::End();
}

void Editor::destroy() {
    delete[] this->inputTextBuffer;
}