#include "core/ShaderRegistry.hpp"
#include "core/logging/Logger.hpp"

ShaderRegistry::ShaderRegistry() {
    initialized = false;
    loggerPtr = nullptr;
    programs.clear();
    factory_ = [](const char* v, const char* f, const char* n, Logger* l) {
        return new ShaderProgram(v, f, n, l);
    };
}

bool ShaderRegistry::initialize(Logger* _loggerPtr, bool registerDefaults) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Shader Registry", "Shader Registry was already initialized.");
        return false;
    }

    loggerPtr = _loggerPtr;
    programs.clear();

    if (registerDefaults) {
        if (!registerProgram("../shaders/tex.vert", "../shaders/tex.frag", "tex")) return false;
        if (!registerProgram("../shaders/color.vert", "../shaders/color.frag", "color")) return false;
    }

    initialized = true;
    return true;
}


void ShaderRegistry::shutdown() {
    if (!initialized) return;
    programs.clear();
    loggerPtr = nullptr;
    initialized = false;
}


bool ShaderRegistry::registerProgram(const std::string& vertex_file, const std::string& fragment_file, const std::string& programName) {
    if (programName.empty()) {
        if (loggerPtr) loggerPtr->addLog(LogLevel::WARNING, "registerProgram", "Shader name cannot be empty");
        return false;
    }

    if(programs.contains(programName)) {
        if (loggerPtr) loggerPtr->addLog(LogLevel::WARNING, "Shader Registry registerProgram", "Program '" + programName + "' already exists.");
        return false;
    }

    programs.emplace(
        programName,
        std::unique_ptr<ShaderProgram>(
            factory_(vertex_file.c_str(), fragment_file.c_str(), programName.c_str(), loggerPtr)
        )
    );

    return true;
}

ShaderProgram* ShaderRegistry::getProgram(const std::string& programName) const {
    if (!initialized) return nullptr;
    
    auto it = programs.find(programName);
    if (it == programs.end()) return nullptr;

    return it->second.get();
}

void ShaderRegistry::replaceProgram(const std::string &programName, std::unique_ptr<ShaderProgram> newProgram) {
    if (!newProgram) return;

    programs[programName] = std::move(newProgram);
}

const std::unordered_map<std::string, std::unique_ptr<ShaderProgram>>& ShaderRegistry::getPrograms() const {
    return programs;
}

size_t ShaderRegistry::getNumberOfPrograms() const {
    return programs.size();
}

void ShaderRegistry::setFactory(ShaderFactoryFn fn) {
    factory_ = std::move(fn);
}