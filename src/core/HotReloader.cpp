#include "core/HotReloader.hpp"
#include "engine/ShaderProgram.hpp"
#include "core/EditorEngine.hpp"
#include "core/InspectorEngine.hpp"
#include "core/ShaderRegistry.hpp"
#include "engine/Errorlog.hpp"
#include "core/EventDispatcher.hpp"
#include "object/ModelCache.hpp"
#include <fstream>
#include <iostream>

bool HotReloader::initialize() {
    EventDispatcher::Subscribe(EventType::SaveActiveShaderFile, [](const EventPayload& payload) -> bool {
    if (const auto* data = std::get_if<SaveActiveShaderFilePayload>(&payload)) {
        
        Model* model = ModelCache::getModel(data->modelID);
        if (!model || !model->getProgram()) return false;

            std::string progName = model->getProgram()->name;

            if (HotReloader::compile(data->filePath, progName)) {
                InspectorEngine::reloadUniforms(data->modelID); 
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
            out << active->textEditor.GetText();
            out.close();

            EventDispatcher::TriggerEvent(MakeSaveActiveShaderFileEvent(active->filePath, active->modelID));            
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
    return true;
}

void HotReloader::scanSourceFiles(const std::string &sourceCode) {

}