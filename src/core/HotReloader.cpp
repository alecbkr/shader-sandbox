#include "core/HotReloader.hpp"
#include "core/logging/LogSink.hpp"
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
#include <filesystem>
#include "core/input/ContextManager.hpp"

HotReloader::HotReloader() {
    initialized = false;
    loggerPtr = nullptr;
    eventsPtr = nullptr;
    shaderRegPtr = nullptr;
    modelCachePtr = nullptr;
    editorEngPtr = nullptr;
    inspectorEngPtr = nullptr;
}

bool HotReloader::initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, ShaderRegistry* _shaderRegPtr, ModelCache* _modelCachePtr, EditorEngine* _editorEngPtr, InspectorEngine* _inspectorEngPtr, ContextManager* _contextManagerPtr) {
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
    contextManagerPtr = _contextManagerPtr;

    eventsPtr->Subscribe(EventType::SaveActiveShaderFile, [this](const EventPayload& payload) -> bool {
    int activeIdx = editorEngPtr->activeEditor;
    if (activeIdx == -1 || editorEngPtr->editors.empty()) {
        loggerPtr->addLog(LogLevel::WARNING, "HotReloader", "No active shader editor open");
        return false; 
    }
    auto* active = editorEngPtr->editors[activeIdx];
        if (!active) return false;

        std::ofstream out(active->filePath, std::ios::binary);
        if (out.is_open()) {
            out << active->textEditor.GetText();
            out.close();
        }

        std::string targetProgram = "";
        std::string absSavedPath = std::filesystem::weakly_canonical(active->filePath).string();

        for (auto const& [name, prog] : shaderRegPtr->getPrograms()) {
            if (std::filesystem::weakly_canonical(prog->vertPath).string() == absSavedPath ||
                std::filesystem::weakly_canonical(prog->fragPath).string() == absSavedPath) {
                targetProgram = name;
                break;
            }
        }

        if (!targetProgram.empty()) {
            if (this->compile(active->filePath, targetProgram)) {
                inspectorEngPtr->reloadUniforms(active->modelID);
                return true;
            }
        } else {
             loggerPtr->addLog(LogLevel::WARNING, "HotReloader", "Saved file not associated with any program.");
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

bool HotReloader::attemptCompile(const std::string &shaderPath, const std::string &programName) {
    ShaderProgram *oldProgram = shaderRegPtr->getProgram(programName);    

    if (programName.empty()) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "attemptCompile", "Shader name cannot be empty");
        return false;
    }

    if (!oldProgram) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "attemptCompile", "Could not find program name");
        return false;
    }
    auto getAbsPath = [](const std::string& p) -> std::string {
        if (p.empty()) return "";
        try {
            return std::filesystem::weakly_canonical(p).string();
        } catch (...) { 
            return p; 
        }
    };

    std::string vPath = getAbsPath(oldProgram->vertPath);
    std::string fPath = getAbsPath(oldProgram->fragPath);
    std::string normalizedTriggerPath = getAbsPath(shaderPath);

    if (normalizedTriggerPath.find(".vert") != std::string::npos) {
        vPath = normalizedTriggerPath;
    } else if (normalizedTriggerPath.find(".frag") != std::string::npos) {
        fPath = normalizedTriggerPath;
    }

    if (inspectorEngPtr->handleEditShaderProgram(vPath, fPath, programName)) {
        ShaderProgram* updatedProg = shaderRegPtr->getProgram(programName);
        if (updatedProg) {
            updatedProg->vertPath = vPath;
            updatedProg->fragPath = fPath;
        }

        inspectorEngPtr->refreshUniforms();
        loggerPtr->addLog(LogLevel::INFO, "HotReloader", "Successfully hot-reloaded: " + programName);
        return true;
    }
    return false;
}

void HotReloader::scanSourceFiles(const std::string &sourceCode) {

}
