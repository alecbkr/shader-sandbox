#include "core/HotReloader.hpp"
#include "engine/ShaderProgram.hpp"
#include "core/ShaderHandler.hpp"
#include "core/EditorEngine.hpp"
#include "core/InspectorEngine.hpp"
#include "engine/Errorlog.hpp"
#include <fstream>
#include <iostream>

HotReloader::HotReloader(ShaderHandler *handler, InspectorEngine *inspector) 
    : m_handler(handler), m_inspector(inspector) {}

bool HotReloader::compile(const std::string &filepath, const std::string &programName) {
    std::string newSourceCode = readSourceFile(filepath);
    if (newSourceCode.empty()) {
        std::cerr << "[HotReloader] Failed to read source file: " << filepath << std::endl;
        return false;
    }

    bool success = attemptCompile(filepath, programName);
    
    if (success) {
        if (m_inspector) {
            m_inspector->reloadUniforms(programName);
        }
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
    ShaderProgram *oldProgram = m_handler->getProgram(programName);
    
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

    ShaderHandler::replaceProgram(programName, newProgram);
    
    if (oldProgram) {
        oldProgram->kill();
        delete oldProgram;
    }

    return true;
}

void HotReloader::scanSourceFiles(const std::string &sourceCode) {

}