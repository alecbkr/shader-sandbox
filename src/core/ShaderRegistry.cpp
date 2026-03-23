#include "core/ShaderRegistry.hpp"
#include "core/logging/Logger.hpp"
#include "application/Project.hpp"
#include "engine/ShaderProgram.hpp"
#include <memory>

ShaderRegistry::ShaderRegistry() {
    initialized = false;
    loggerPtr = nullptr;
    factory_ = [](const char* v, const char* f, const char* n, Logger* l) {
        return new ShaderProgram(v, f, n, l);
    };
}

bool ShaderRegistry::initialize(Logger* _loggerPtr, Project* _project, bool registerDefaults) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Shader Registry", "Shader Registry was already initialized.");
        return false;
    }

    loggerPtr = _loggerPtr;
    project = _project;
    project->programs.clear();

    if (registerDefaults) {
        if (!registerProgram(project->projectShadersDir / "tex.vert", project->projectShadersDir / "tex.frag", "tex")) return false;
        if (!registerProgram(project->projectShadersDir / "color.vert", project->projectShadersDir / "color.frag", "color")) return false;
        if (!registerProgram(project->projectShadersDir / "gridplane.vert", project->projectShadersDir / "gridplane.frag", "gridplane")) return false;
        if (!registerProgram(project->projectShadersDir / "skybox.vert", project->projectShadersDir / "skybox.frag", "skybox")) return false;
        if (!registerProgram(project->projectShadersDir / "instance.vert", project->projectShadersDir / "instance.frag", "instance")) return false;
    }

    initialized = true;
    return true;
}


void ShaderRegistry::shutdown() {
    if (!initialized) return;
    project->programs.clear();
    loggerPtr = nullptr;
    initialized = false;
}


bool ShaderRegistry::registerProgram(const std::filesystem::path& vertex_file, const std::filesystem::path& fragment_file, const std::string& programName) {
    if (programName.empty()) {
        if (loggerPtr) loggerPtr->addLog(LogLevel::WARNING, "registerProgram", "Shader name cannot be empty");
        return false;
    }

    if(project->programs.contains(programName)) {
        if (loggerPtr) loggerPtr->addLog(LogLevel::WARNING, "Shader Registry registerProgram", "Program '" + programName + "' already exists.");
        return false;
    }

    project->programs.emplace(
        programName,
        std::unique_ptr<ShaderProgram>(
            factory_(vertex_file.string().c_str(), fragment_file.string().c_str(), programName.c_str(), loggerPtr)
        )
    );

    return true;
}

ShaderProgram* ShaderRegistry::getProgram(const std::string& programName) const {
    if (!initialized) return nullptr;
    
    auto it = project->programs.find(programName);
    if (it == project->programs.end()) return nullptr;

    return it->second.get();
}

void ShaderRegistry::replaceProgram(const std::string &programName, std::unique_ptr<ShaderProgram> newProgram) {
    if (!newProgram) return;

    project->programs[programName] = std::move(newProgram);
}
void ShaderRegistry::replaceProgram(const std::string& vertex_file, const std::string& fragment_file, const std::string& programName) {
    auto s = std::unique_ptr<ShaderProgram>(
            factory_(vertex_file.c_str(), fragment_file.c_str(), programName.c_str(), loggerPtr)
        );

    replaceProgram(programName, std::move(s));
}


const std::unordered_map<std::string, std::unique_ptr<ShaderProgram>>& ShaderRegistry::getPrograms() const {
    return project->programs;
}

size_t ShaderRegistry::getNumberOfPrograms() const {
    return project->programs.size();
}

void ShaderRegistry::setFactory(ShaderFactoryFn fn) {
    factory_ = std::move(fn);
}
