#include "core/HotReloader.hpp"
#include "engine/ShaderProgram.hpp"
#include "core/EditorEngine.hpp"
#include "core/InspectorEngine.hpp"
#include "core/ShaderRegistry.hpp"
#include "engine/Errorlog.hpp"
#include "core/EventDispatcher.hpp"
#include <fstream>
#include <iostream>

bool HotReloader::initialize() {
    EventDispatcher::Subscribe(EventType::SaveActiveShaderFile, [](const EventPayload& payload) -> bool {
        if (const auto* data = std::get_if<SaveActiveShaderFilePayload>(&payload)) {
            if (HotReloader::compile(data->filePath, data->programName)) {
                EventDispatcher::TriggerEvent(MakeReloadShaderEvent(data->programName));
                return true;
            }
        }
        return false; 
    });
    return true;
}

void HotReloader::update() {
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S)) {
        
        int activeIdx = EditorEngine::activeEditor;
        if (activeIdx != -1) {
            auto* active = EditorEngine::editors[activeIdx];
            
            std::ofstream out(active->filePath, std::ios::binary);
            out << active->inputTextBuffer;
            out.close();

            EventDispatcher::TriggerEvent(MakeSaveActiveShaderFileEvent(active->filePath, "program"));
            
            Logger::addLog(LogLevel::INFO, "HotReloader", "Performing Hot Reloading");
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
    ShaderProgram *oldProgram = ShaderRegistry::getProgram(programName);

    std::string vPath = (oldProgram) ? oldProgram->vertPath : "../shaders/default.vert";

    if (programName == "") {
        Logger::addLog(LogLevel::ERROR, "attemptCompile", "Shader name cannot be empty");
        return false;
    }
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

    ShaderRegistry::replaceProgram(programName, newProgram);
    InspectorEngine::reloadUniforms(programName);
    
    if (oldProgram) {
        oldProgram->kill();
        delete oldProgram;
    }

    return true;
}

void HotReloader::scanSourceFiles(const std::string &sourceCode) {

}
