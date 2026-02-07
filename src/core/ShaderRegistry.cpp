#include "core/ShaderRegistry.hpp"
#include "core/logging/Logger.hpp"

ShaderRegistry::ShaderRegistry() {
    initialized = false;
    loggerPtr = nullptr;
    programs.clear();
}

bool ShaderRegistry::initialize(Logger* _loggerPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Shader Registry", "Shader Registry was already initialized.");
        return false;
    }

    programs.clear();

    if (!registerProgram("../shaders/tex.vert", "../shaders/tex.frag", "tex")) {
        return false;
    };
    if (!registerProgram("../shaders/color.vert", "../shaders/color.frag", "color")) {
        return false;
    }

    loggerPtr = _loggerPtr;

    initialized = true;
    return true;
}

void ShaderRegistry::shutdown() {
    if (!initialized) return;
    loggerPtr = nullptr;
    programs.clear();
    initialized = false;
}

bool ShaderRegistry::registerProgram(const std::string& vertex_file, const std::string& fragment_file, const std::string& programName) {
    if (programName == "") {
        loggerPtr->addLog(LogLevel::WARNING, "registerProgram", "Shader name cannot be empty");
        return false;
    }
    ShaderProgram *newProgram = new ShaderProgram(vertex_file.c_str(), fragment_file.c_str(), programName.c_str(), loggerPtr);
    auto [it, inserted] = programs.emplace(programName, newProgram);
    //auto [it, inserted] = programs.emplace( programName, ShaderProgram(vertex_file.c_str(), fragment_file.c_str(), programName.c_str()));

    if (!inserted) {
        std::cerr << "ShaderRegistry::registerProgram: program '" << programName << "' already exists\n";
        delete newProgram;
    }

    return inserted;
}

ShaderProgram* ShaderRegistry::getProgram(const std::string& programName) {
    if (!ShaderRegistry::initialized) return nullptr;
    auto programPair = programs.find(programName);
    if (programPair == programs.end()) {
        return nullptr;
    }

    //return &programPair->second;
    return programPair->second;
}

void ShaderRegistry::replaceProgram(const std::string &programName, ShaderProgram *newProgram) {
    auto it = programs.find(programName);
    if (it != programs.end()) {
        ShaderProgram* oldProg = it->second;
        it->second = newProgram;
        delete oldProg; 
    } else {
        programs[programName] = newProgram;
    }
}

std::unordered_map<std::string, ShaderProgram*>& ShaderRegistry::getPrograms() {
    return programs;
}

size_t ShaderRegistry::getNumberOfPrograms() {
    return programs.size();
}
