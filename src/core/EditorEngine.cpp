#include "EditorEngine.hpp"
#include <fstream>
#include <filesystem>
#include "core/logging/Logger.hpp"
#include "core/EventDispatcher.hpp"
#include "object/ModelCache.hpp"

std::string getFileContents(std::string filename) {
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

Editor::Editor(std::string filePath, std::string fileName, unsigned int modelID) {
    this->filePath = filePath;
    this->fileName = fileName;
    this->modelID = modelID;

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

    std::string content = getFileContents(filePath);
    this->textEditor.SetText(content);
}

void Editor::destroy() {
    delete this;
}

EditorEngine::EditorEngine() {
    initialized = false;
    loggerPtr = nullptr;
    eventsPtr = nullptr;
    modelCachePtr = nullptr;
    editors.clear();
    activeEditor = 0;
}

bool EditorEngine::initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, ModelCache* _modelCachePtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Editor Engine Initialization", "Editor Engine was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    eventsPtr = _eventsPtr;
    modelCachePtr = _modelCachePtr;
    editors.clear();
    activeEditor = 0;

    eventsPtr->Subscribe(EventType::OpenFile, std::bind(&EditorEngine::spawnEditor, this, std::placeholders::_1));
    eventsPtr->Subscribe(EventType::NewFile, std::bind(&EditorEngine::spawnEditor, this, std::placeholders::_1));
    eventsPtr->Subscribe(EventType::RenameFile, std::bind(&EditorEngine::renameEditor, this, std::placeholders::_1));
    eventsPtr->Subscribe(EventType::DeleteFile, std::bind(&EditorEngine::deleteEditor, this, std::placeholders::_1));
    
    initialized = true;
    return true;
}

void EditorEngine::shutdown() {
    if (!initialized) return;
    loggerPtr = nullptr;
    eventsPtr = nullptr;
    modelCachePtr = nullptr;
    editors.clear();
    activeEditor = 0;
    initialized = false;
}

bool EditorEngine::spawnEditor(const EventPayload& payload) {
    if (const auto* data = std::get_if<OpenFilePayload>(&payload)) {
        unsigned int linkedID = data->modelID;

        if (linkedID == 0) {
            for (auto const& [id, model] : modelCachePtr->modelIDMap) {
                if (model->getProgram()) {
                    if (model->getProgram()->fragPath == data->filePath || 
                        model->getProgram()->vertPath == data->filePath) {
                        linkedID = id;
                        break;
                    }
                }
            }
        }
        if (!data->filePath.empty()) {
            editors.push_back(new Editor(data->filePath, data->fileName, linkedID));
        } else {
            editors.push_back(new Editor("../shaders/tex.frag", "tex.frag", linkedID));
        }
        return true;
    } else if (std::get_if<std::monostate>(&payload)) {
        try {
            const std::string fileName = "Untitled " + findNextUntitledNumber();
            const std::string filePath = "../shaders/" + fileName;
            createFile(filePath);
            editors.push_back(new Editor(filePath, fileName, 0));
            return true;
        } catch (const std::filesystem::filesystem_error& e) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "EditorEngine::createFile", std::string("Filesystem error: ") + e.what());
        }
    } else {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "spawnEditor", "Invalid Payload Type");
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
        loggerPtr->addLog(LogLevel::LOG_ERROR, "renameEditor", "Invalid Payload Type");
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
        loggerPtr->addLog(LogLevel::LOG_ERROR, "renameEditor", "Invalid Payload Type");
    }
    return false;
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
