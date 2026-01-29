#include "EditorEngine.hpp"

#include <fstream>

#include "logging/Logger.hpp"
#include "core/EventDispatcher.hpp"


std::vector<Editor*> EditorEngine::editors{};
int EditorEngine::activeEditor = -1;

Editor::Editor(std::string filePath, std::string fileName) {
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
}

void Editor::destroy() {
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
        } else {
            editors.push_back(new Editor("../shaders/texture.frag", "texture.frag"));
        }
    } else if (std::get_if<std::monostate>(&payload)) {
        try {
            const std::string fileName = "Untitled " + findNextUntitledNumber();
            const std::string filePath = "../shaders/" + fileName;
            createFile(filePath);
            editors.push_back(new Editor(filePath, fileName));
        } catch (const std::filesystem::filesystem_error& e) {
            Logger::addLog(LogLevel::ERROR, "EditorEngine::createFile", std::string("Filesystem error: ") + e.what());
        }
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

std::string EditorEngine::findNextUntitledNumber() {
    int i = 0;
    while (std::filesystem::exists("../shaders/Untitled " + std::to_string(i))) i++;
    return std::to_string(i);
}
