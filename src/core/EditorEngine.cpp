#include "EditorEngine.hpp"

#include <fstream>
#include <iostream>

Editor::Editor(unsigned int _bufferSize) {
    inputTextBuffer = new char[_bufferSize];
    strcpy(inputTextBuffer, EditorEngine::getFileContents("../shaders/texture.frag").c_str());
    bufferSize = _bufferSize;
    lineCount = 1;

    int i = 0;
    while (inputTextBuffer[i] != '\0') {
        if (inputTextBuffer[i] == '\n') lineCount++;
        i++;
    }

    previousTextLen = i;
}

void Editor::destroy() {
    free(inputTextBuffer);
    delete this;
}

void EditorEngine::spawnEditor(unsigned int bufferSize) {
    Editor *editor = new Editor(bufferSize);
    editors.push_back(editor);
}

std::string EditorEngine::getFileContents(const char *filename) {
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

int EditorEngine::EditorInputCallback(ImGuiInputTextCallbackData* data) {
    EditorUI* ui = static_cast<EditorUI*>(data->UserData);

    if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
        updatePropertiesDueToMassDelete(data, ui);
        matchBrace(data, ui);
        updateLineCount(data, ui);
        updatePropertiesDueToMassInsert(data, ui);
    }

    ui->previousTextLen = data->BufTextLen;

    return 0;
}

void EditorEngine::updatePropertiesDueToMassDelete(ImGuiInputTextCallbackData* data, EditorUI* ui) {
    if (data->BufTextLen < ui->previousTextLen - 1) {
        int newLineCount = 1;
        int i = 0;
        while (data->Buf[i] != '\0') {
            if (data->Buf[i] == '\n') newLineCount++;
            i++;
        }

        ui->lineCount = newLineCount;
    }
}

void EditorEngine::updatePropertiesDueToMassInsert(ImGuiInputTextCallbackData* data, EditorUI* ui) {
    if (data->BufTextLen > ui->previousTextLen + 1) {
        int newLineCount = 1;
        int i = 0;
        while (data->Buf[i] != '\0') {
            if (data->Buf[i] == '\n') newLineCount++;
            i++;
        }

        ui->lineCount = newLineCount;
    }
}

void EditorEngine::matchBrace(ImGuiInputTextCallbackData* data, EditorUI* ui) {
    if (data->CursorPos < 2) return;

    bool newLineInserted = data->BufTextLen == ui->previousTextLen + 1 && data->Buf[data->CursorPos - 1] == '\n';
    bool openBraceExists = data->Buf[data->CursorPos - 2] == '{';

    if (newLineInserted && openBraceExists) {
        int backIndex = data->CursorPos - 3;
        std::string tabString;
        while (backIndex >= 0 && data->Buf[backIndex] != '\n') {
            if (data->Buf[backIndex] == '\t') {
                tabString += "\t";
            }
            backIndex--;
        }

        std::string closeBrace = tabString + "\t\n" + tabString + "}";

        data->InsertChars(data->CursorPos, closeBrace.c_str());

        data->CursorPos -= 2 + tabString.length();
        ui->lineCount+=2;
    }
}

void EditorEngine::updateLineCount(ImGuiInputTextCallbackData* data, EditorUI* ui) {
    char* previousBuffer = ui->inputTextBuffer;

    if (data->BufTextLen == ui->previousTextLen - 1 && previousBuffer[data->CursorPos] == '\n') {
        ui->lineCount--;
    }

    if (data->CursorPos < 1) return;

    if (data->BufTextLen == ui->previousTextLen + 1 && data->Buf[data->CursorPos - 1] == '\n') {
        int backIndex = data->CursorPos - 2;
        std::string tabString;
        while (backIndex >= 0 && data->Buf[backIndex] != '\n') {
            if (data->Buf[backIndex] == '\t') {
                tabString += "\t";
            }
            backIndex--;
        }

        data->InsertChars(data->CursorPos, tabString.c_str());

        ui->lineCount++;
    }

}