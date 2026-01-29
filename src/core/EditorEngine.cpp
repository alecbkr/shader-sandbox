#include "EditorEngine.hpp"

#include <fstream>

#include "logging/Logger.hpp"
#include "core/EventDispatcher.hpp"


std::vector<Editor*> EditorEngine::editors{};
int EditorEngine::activeEditor = -1;

Editor::Editor(std::string filePath, std::string fileName) {
    //this->bufferSize = bufferSize;
    //this->inputTextBuffer = new char[bufferSize];
    this->filePath = filePath;
    this->fileName = fileName;

    auto lang = TextEditor::LanguageDefinition::GLSL();
    // Will probably need to find the entire list of glsl keywords
    const char* const glslKeywords[] = {
        "vec2", "vec3", "vec4", "mat2", "mat3", "mat4", "sampler2D", "samplerCube", 
        "out", "in", "uniform", "layout"
    };

    for (auto& k : glslKeywords)
        lang.mKeywords.insert(k);

    this->textEditor.SetLanguageDefinition(lang);
    auto palette = TextEditor::GetDarkPalette();
    this->textEditor.SetShowWhitespaces(false);

    // Change palette colors here
    palette[(int)TextEditor::PaletteIndex::Identifier] = 0xff9cdcfe;
    palette[(int)TextEditor::PaletteIndex::Keyword] = 0xffd197d9;
    textEditor.SetPalette(palette);

    std::string content = EditorEngine::getFileContents(filePath);
    this->textEditor.SetText(content);
    /*
    strcpy(this->inputTextBuffer, EditorEngine::getFileContents(filePath).c_str());
    this->lineCount = 1;

    int i = 0;
    while (this->inputTextBuffer[i] != '\0') {
        if (this->inputTextBuffer[i] == '\n') this->lineCount++;
        i++;
    }

    this->previousTextLen = i;
    */
}

void Editor::destroy() {
    //free(inputTextBuffer);
    delete this;
}

bool EditorEngine::initialize() {
    EventDispatcher::Subscribe(EventType::OpenFile, spawnEditor);
    EventDispatcher::Subscribe(EventType::NewFile, spawnEditor);
    EventDispatcher::Subscribe(EventType::RenameFile, renameEditor);
    EventDispatcher::Subscribe(EventType::DeleteFile, deleteEditor);
    return true;
}

bool EditorEngine::spawnEditor(const EventPayload& payload) {
    if (const auto* data = std::get_if<OpenFilePayload>(&payload)) {
        if (!data->filePath.empty()) {
            editors.push_back(new Editor(data->filePath, data->fileName));
            //editors.push_back(new Editor(2056, data->filePath, data->fileName));
        } else {
            editors.push_back(new Editor("../shaders/texture.frag", "texture.frag"));
            //editors.push_back(new Editor(2056, "../shaders/texture.frag", "texture.frag"));
        }
    } else if (std::get_if<std::monostate>(&payload)) {
        editors.push_back(new Editor("", ""));
        //editors.push_back(new Editor(2056, "", ""));
    } else {
        Logger::addLog(LogLevel::ERROR, "spawnEditor", "Invalid Payload Type");
    }

    return false;
}

bool EditorEngine::renameEditor(const EventPayload& payload) {
    if (const auto* data = std::get_if<RenameFilePayload>(&payload)) {
        for (auto* editor : editors) {
            if (editor->fileName == data->oldName) {
                editor->fileName = data->newName;

                std::filesystem::path newPath = editor->filePath;
                newPath.replace_filename(data->newName);
                editor->filePath = newPath.string();
            }
        }
    } else {
        Logger::addLog(LogLevel::ERROR, "renameEditor", "Invalid Payload Type");
    }
    return false;
}

bool EditorEngine::deleteEditor(const EventPayload& payload) {
    if (const auto* data = std::get_if<DeleteFilePayload>(&payload)) {
        for (int i = 0; i < editors.size(); i++) {
            if (editors[i]->fileName == data->fileName) {
                editors[i]->destroy();
                editors.erase(editors.begin() + i);

                if (editors.empty()) activeEditor = -1;
                i--;
            }
        }
    } else {
        Logger::addLog(LogLevel::ERROR, "renameEditor", "Invalid Payload Type");
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

void EditorEngine::createFile(const std::string& filePath) {
    std::ofstream outfile(filePath);
    outfile << "#version 330 core\n\n";
    outfile << "void main() {\n\t\n}";
    outfile.close();
}
/*
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
*/