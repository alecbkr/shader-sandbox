#include "core/HotReloader.hpp"
#include "engine/ShaderProgram.hpp"
#include "engine/Errorlog.hpp"
#include <fstream>
#include <iostream>
#include "core/logging/Logger.hpp"
#include "core/EventDispatcher.hpp"
#include "core/ShaderRegistry.hpp"
#include "object/ModelCache.hpp"
#include "core/EditorEngine.hpp"
#include "core/InspectorEngine.hpp"

HotReloader::HotReloader() {
    initialized = false;
    loggerPtr = nullptr;
    eventsPtr = nullptr;
    shaderRegPtr = nullptr;
    modelCachePtr = nullptr;
    editorEngPtr = nullptr;
    inspectorEngPtr = nullptr;
}

bool HotReloader::initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, ShaderRegistry* _shaderRegPtr, ModelCache* _modelCachePtr, EditorEngine* _editorEngPtr, InspectorEngine* _inspectorEngPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Hot Reloader Initialization", "Hot Reloader was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    eventsPtr = _eventsPtr;
    shaderRegPtr = _shaderRegPtr;
    modelCachePtr = _modelCachePtr;
    editorEngPtr = _editorEngPtr;
    inspectorEngPtr = _inspectorEngPtr;

    eventsPtr->Subscribe(EventType::SaveActiveShaderFile, [this](const EventPayload& payload) -> bool {
    if (const auto* data = std::get_if<SaveActiveShaderFilePayload>(&payload)) {
        
        Model* model = modelCachePtr->getModel(data->modelID);
        if (!model || !model->getProgram()) return false;

            std::string progName = model->getProgram()->name;

            if (compile(data->filePath, progName)) {
                inspectorEngPtr->reloadUniforms(data->modelID); 
                return true;
            }
        }
        return false;
    });

    initialized = true;
    return true;
}

void HotReloader::shutdown() {
    if (!initialized) return;
    loggerPtr = nullptr;
    eventsPtr = nullptr;
    shaderRegPtr = nullptr;
    modelCachePtr = nullptr;
    editorEngPtr = nullptr;
    inspectorEngPtr = nullptr;
    initialized = false;
}

void HotReloader::update() {
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S)) {
        int activeIdx = editorEngPtr->activeEditor;
        if (activeIdx != -1) {
            auto* active = editorEngPtr->editors[activeIdx];
            
            std::ofstream out(active->filePath, std::ios::binary);
            out << active->textEditor.GetText();
            out.close();

            eventsPtr->TriggerEvent(MakeSaveActiveShaderFileEvent(active->filePath, active->modelID));            
            loggerPtr->addLog(LogLevel::INFO, "HotReloader", "Performing Hot Reloading");
        }
    }
}

bool HotReloader::compile(const std::string &filepath, const std::string &programName) {
    std::string newSourceCode = readSourceFile(filepath);
    if (newSourceCode.empty()) {
        std::cerr << "[HotReloader] Failed to read source file: " << filepath << std::endl;
        return false;
    }
    
    if (attemptCompile(filepath, programName)) {
        return true;
    }
    return false;
}

std::string HotReloader::readSourceFile(const std::string &filepath) {
    std::ifstream in(filepath, std::ios::binary);
    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return contents;
    }
    return "";
}

bool HotReloader::attemptCompile(const std::string &fragShaderPath, const std::string &programName) {
    ShaderProgram *oldProgram = shaderRegPtr->getProgram(programName);
    std::string vPath = (oldProgram) ? oldProgram->vertPath : "../shaders/default.vert";

    ShaderProgram *newProgram = new ShaderProgram(
        vPath.c_str(), 
        fragShaderPath.c_str(),
        programName.c_str()
    );

    if (!newProgram->isCompiled()) {
        if (oldProgram && oldProgram->isCompiled()) {
            oldProgram->use();
        } else {
            glUseProgram(0);
        }
        delete newProgram;
        return false;
    }

    shaderRegPtr->replaceProgram(programName, newProgram);
    return true;
}

void HotReloader::scanSourceFiles(const std::string &sourceCode) {

}