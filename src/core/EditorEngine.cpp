#include "EditorEngine.hpp"
#include <fstream>
#include <filesystem>
#include <limits>
#include "core/logging/Logger.hpp"
#include "core/EventDispatcher.hpp"
#include "object/ModelCache.hpp"
#include "core/ShaderRegistry.hpp"
#include "application/SettingsStyles.hpp"

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

Editor::Editor(std::string filePath, std::string fileName, unsigned int modelID, SettingsStyles* styles) {
    this->filePath = std::move(filePath);
    this->fileName = std::move(fileName);
    this->modelID = modelID;
    this->stylesPtr = styles;
    seenPaletteVersion = std::numeric_limits<u32>::max();
    
    TextEditor::LanguageDefinition lang = TextEditor::LanguageDefinition::GLSL();
    this->textEditor.SetLanguageDefinition(lang);
    this->textEditor.SetShowWhitespaces(false);
    applyPaletteIfOutdated();

    this->textEditor.SetText(getFileContents(this->filePath));
}

void Editor::render() {
    applyPaletteIfOutdated();
    textEditor.Render("ShaderEditor");
}

void Editor::applyPaletteIfOutdated() {
    if (stylesPtr->paletteVersion != seenPaletteVersion) {
        if (stylesPtr->hasLoadedPalette) {
            TextEditor::Palette pal;
            for (int i = 0; i < (int)TextEditor::PaletteIndex::Max; ++i) {
                pal[i] = ImGui::ColorConvertFloat4ToU32(stylesPtr->editorPalette[i]);
            }
            textEditor.SetPalette(pal);
            seenPaletteVersion = stylesPtr->paletteVersion;
        } else {
            textEditor.SetPalette(TextEditor::GetDarkPalette());
        }
    }
}

void Editor::destroy() {
    delete this;
}

EditorEngine::EditorEngine() {
    initialized = false;
    loggerPtr = nullptr;
    eventsPtr = nullptr;
    modelCachePtr = nullptr;
    shaderRegPtr = nullptr;
    stylesPtr = nullptr;
    editors.clear();
    activeEditor = 0;
}

bool EditorEngine::initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, ModelCache* _modelCachePtr, ShaderRegistry* _shaderRegPtr, SettingsStyles* styles) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Editor Engine Initialization", "Editor Engine was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    eventsPtr = _eventsPtr;
    modelCachePtr = _modelCachePtr;
    shaderRegPtr = _shaderRegPtr;
    stylesPtr = styles;
    editors.clear();
    activeEditor = 0;

    eventsPtr->Subscribe(EventType::OpenFile, std::bind(&EditorEngine::spawnEditor, this, std::placeholders::_1));
    eventsPtr->Subscribe(EventType::NewFile, std::bind(&EditorEngine::spawnEditor, this, std::placeholders::_1));
    eventsPtr->Subscribe(EventType::RenameFile, std::bind(&EditorEngine::renameEditor, this, std::placeholders::_1));
    eventsPtr->Subscribe(EventType::DeleteFile, std::bind(&EditorEngine::deleteEditor, this, std::placeholders::_1));

    // syncing styles with settings if no loaded settings
    if (!stylesPtr->hasLoadedPalette) {
        const auto& dark = TextEditor::GetDarkPalette();

        for (int i = 0; i < (int)TextEditor::PaletteIndex::Max; ++i) {
            stylesPtr->editorPalette[i] = ImGui::ColorConvertU32ToFloat4(dark[i]);
        }

        stylesPtr->hasLoadedPalette = true;
    }
    
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

                ShaderProgram* modelProgram = shaderRegPtr->getProgram(model->getProgramID());
                if (modelProgram != nullptr) {
                    if (modelProgram->fragPath == data->filePath || 
                        modelProgram->vertPath == data->filePath) {
                        linkedID = id;
                        break;
                    }
                }
            }
        }
        if (!data->filePath.empty()) {
            editors.push_back(new Editor(data->filePath, data->fileName, linkedID, stylesPtr));
        } else {
            editors.push_back(new Editor("../shaders/tex.frag", "tex.frag", linkedID, stylesPtr));
        }
        return true;
    } else if (std::get_if<std::monostate>(&payload)) {
        try {
            const std::string fileName = "Untitled " + findNextUntitledNumber();
            const std::string filePath = "../shaders/" + fileName;
            createFile(filePath);
            editors.push_back(new Editor(filePath, fileName, 0, stylesPtr));
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
