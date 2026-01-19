#include "EditorEngine.hpp"

#include <fstream>
#include "logging/Logger.hpp"
#include "core/EventDispatcher.hpp"


std::vector<Editor*> EditorEngine::editors{};
int EditorEngine::activeEditor = -1;

Editor::Editor(unsigned int bufferSize, std::string filePath, std::string fileName) {
    this->bufferSize = bufferSize;
    this->inputTextBuffer = new char[bufferSize];
    this->filePath = filePath;
    this->fileName = fileName;

    strcpy(this->inputTextBuffer, EditorEngine::getFileContents(filePath).c_str());

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
    EventDispatcher::Subscribe(EventType::NewFile, spawnEditor);
    return true;
}

bool EditorEngine::spawnEditor(const EventPayload& payload) {
    if (const auto* data = std::get_if<OpenFilePayload>(&payload)) {
        if (!data->filePath.empty()) {
            editors.push_back(new Editor(2056, data->filePath, data->fileName));
        } else {
            editors.push_back(new Editor(2056, "../shaders/texture.frag", "texture.frag"));
        }
    } else if (std::get_if<std::monostate>(&payload)) {
        editors.push_back(new Editor(2056, "", ""));
    } else {
        Logger::addLog(LogLevel::ERROR, "spawnEditor", "Invalid Payload Type");
        return false;
    }

    return true;
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

bool EditorEngine::createFile(const std::string& filePath) {
    if (!std::filesystem::exists("../shaders/")) {
        Logger::addLog(LogLevel::ERROR, "EditorEngine::createFile", "Shader directory does not exist.", "../shaders/");
        return false;
    }

    std::ofstream outfile(filePath);
    if (outfile.is_open()) {
        outfile << "#version 330 core\n\n";
        outfile << "void main() {\n\t\n}";
        outfile.close();

        return true;
    }

    Logger::addLog(LogLevel::ERROR, "EditorEngine::createFile", "Invalid File.", filePath);
    return false;
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