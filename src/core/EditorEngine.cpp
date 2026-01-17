#include "EditorEngine.hpp"

#include <fstream>
#include "logging/Logger.hpp"
#include "core/EventDispatcher.hpp"


std::vector<Editor*> EditorEngine::editors{};
int EditorEngine::activeEditor = -1;

Editor::Editor(unsigned int bufferSize, std::string filePath, std::string fileName) {
    this->inputTextBuffer = new char[bufferSize];
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

void Editor::destroy() {
    free(inputTextBuffer);
    delete this;
}

bool EditorEngine::initialize() {
    EventDispatcher::Subscribe(EventType::OpenFile, spawnEditor);
    return true;
}

bool EditorEngine::spawnEditor(const EventPayload& payload) {
    if (const auto* data = std::get_if<OpenFilePayload>(&payload)) {
        if (!data->filePath.empty()) {
            Editor *editor = new Editor(1024, data->filePath, data->fileName);
            editors.push_back(editor);
        } else {
            Editor *editor = new Editor(1024, "../shaders/texture.frag", "texture.frag");
            editors.push_back(editor);
        }
    } else {
        Logger::addLog(LogLevel::ERROR, "spawnEditor", "Invalid Payload Type");
    }

    return false;
}

std::string EditorEngine::getFileContents(std::string filename) {
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
    Editor* editor = static_cast<Editor*>(data->UserData);

    if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
        updatePropertiesDueToMassDelete(data, editor);
        matchBrace(data, editor);
        updateLineCount(data, editor);
        updatePropertiesDueToMassInsert(data, editor);
    }

    editor->previousTextLen = data->BufTextLen;

    return 0;
}

void EditorEngine::updatePropertiesDueToMassDelete(ImGuiInputTextCallbackData* data, Editor* editor) {
    if (data->BufTextLen < editor->previousTextLen - 1) {
        int newLineCount = 1;
        int i = 0;
        while (data->Buf[i] != '\0') {
            if (data->Buf[i] == '\n') newLineCount++;
            i++;
        }

        editor->lineCount = newLineCount;
    }
}

void EditorEngine::updatePropertiesDueToMassInsert(ImGuiInputTextCallbackData* data, Editor* editor) {
    if (data->BufTextLen > editor->previousTextLen + 1) {
        int newLineCount = 1;
        int i = 0;
        while (data->Buf[i] != '\0') {
            if (data->Buf[i] == '\n') newLineCount++;
            i++;
        }

        editor->lineCount = newLineCount;
    }
}

void EditorEngine::matchBrace(ImGuiInputTextCallbackData* data, Editor* editor) {
    if (data->CursorPos < 2) return;

    bool newLineInserted = data->BufTextLen == editor->previousTextLen + 1 && data->Buf[data->CursorPos - 1] == '\n';
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
        editor->lineCount+=2;
    }
}

void EditorEngine::updateLineCount(ImGuiInputTextCallbackData* data, Editor* editor) {
    char* previousBuffer = editor->inputTextBuffer;

    if (data->BufTextLen == editor->previousTextLen - 1 && previousBuffer[data->CursorPos] == '\n') {
        editor->lineCount--;
    }

    if (data->CursorPos < 1) return;

    if (data->BufTextLen == editor->previousTextLen + 1 && data->Buf[data->CursorPos - 1] == '\n') {
        int backIndex = data->CursorPos - 2;
        std::string tabString;
        while (backIndex >= 0 && data->Buf[backIndex] != '\n') {
            if (data->Buf[backIndex] == '\t') {
                tabString += "\t";
            }
            backIndex--;
        }

        data->InsertChars(data->CursorPos, tabString.c_str());

        editor->lineCount++;
    }

}