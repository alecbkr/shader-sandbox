#include "core/ShaderRegistry.hpp"

std::unordered_map<std::string, ShaderProgram *> ShaderRegistry::programs;
bool ShaderRegistry::initialized = false;

bool ShaderRegistry::initialize() {
    if (!registerProgram("../shaders/3d.vert", "../shaders/texture.frag", "default")) {
        return false;
    };

    ShaderRegistry::initialized = true;
    return true;
}

bool ShaderRegistry::registerProgram(const std::string& vertex_file, const std::string& fragment_file, const std::string& programName) {
    ShaderProgram *newProgram = new ShaderProgram(vertex_file.c_str(), fragment_file.c_str(), programName.c_str());
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
        std::cout << "Get Program: No program with name " << programName <<" found in the Shader Handler programs" << std::endl;
        return nullptr;
    }

    //return &programPair->second;
    return programPair->second;
}

void ShaderRegistry::replaceProgram(const std::string &programName, ShaderProgram *newProgram){
    if (!ShaderRegistry::initialized) return;
    auto it = programs.find(programName);
    if (it == programs.end()){
        delete newProgram;
        return;
    }
    it->second = newProgram;
}

std::unordered_map<std::string, ShaderProgram*>& ShaderRegistry::getPrograms() {
    return programs;
}

size_t ShaderRegistry::getNumberOfPrograms() {
    return programs.size();
}
