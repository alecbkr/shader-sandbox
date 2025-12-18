#include "engine/ShaderProgram.hpp"
#include "ShaderHandler.hpp"
#include "EditorEngine.hpp"
#include "HotReloader.hpp"
#include "InspectorEngine.hpp"
#include "engine/Errorlog.hpp"
#include <fstream>

const std::string vertShaderPath = "../shaders/3d.vert";
const std::string programName = "default";

HotReloader::HotReloader(ShaderHandler *handler, InspectorEngine *inspector) : m_handler(handler), m_inspector(inspector){}

bool HotReloader::compile(const std::string &filepath){
    std::string newSourceCode = readSourceFile(filepath);
    if (newSourceCode.empty()){
        std::cerr << "failed to read source file." << std::endl;
        return false;
    }

    scanSourceFiles(newSourceCode);
    bool success = attemptCompile(filepath);
    return success;
}

std::string HotReloader::readSourceFile(const std::string &filepath){
    std::ifstream in(filepath, std::ios::binary);
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

void HotReloader::scanSourceFiles(const std::string &sourceCode){

}

bool HotReloader::attemptCompile(const std::string &fragShaderPath){
    ShaderProgram *newProgram = new ShaderProgram(
        vertShaderPath.c_str(),
        fragShaderPath.c_str(),
        programName.c_str()
    );

    if (!newProgram->isCompiled()){
        ShaderProgram *currentProgram = m_handler->getProgram(programName);
        if (currentProgram && currentProgram->isCompiled()){
            currentProgram->use();
        } else {
            glUseProgram(0);
        }

        delete newProgram;
        return false;
    }

    ShaderProgram *oldProgram = m_handler->getProgram(programName);
    ShaderHandler::replaceProgram(programName, newProgram);
    
    if (m_inspector){
        m_inspector->reloadUniforms(programName);
    } 
    if (oldProgram){
        oldProgram ->kill();
        delete oldProgram;
    }

    return true;
}