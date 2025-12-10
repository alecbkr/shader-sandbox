#include "ShaderHandler.hpp"

std::unordered_map<std::string, ShaderProgram> ShaderHandler::programs{};

ShaderHandler::ShaderHandler() {
    registerProgram("../shaders/3d.vert", "../shaders/default.frag", "default");
}

bool ShaderHandler::registerProgram(const std::string& vertex_file, const std::string& fragment_file, const std::string& programName) {
    auto [it, inserted] = programs.emplace( programName, ShaderProgram(vertex_file.c_str(), fragment_file.c_str(), programName.c_str()));

    if (!inserted) {
        std::cerr << "ShaderHandler::registerProgram: program '" << programName << "' already exists\n";
    }

    return inserted;
}

ShaderProgram* ShaderHandler::getProgram(const std::string& programName) {
    auto programPair = programs.find(programName);
    if (programPair == programs.end()) {
        std::cout << "Get Program: No program with that name found in the Shader Handler programs" << std::endl;
        return nullptr;
    }

    return &programPair->second;
}

std::unordered_map<std::string, ShaderProgram>& ShaderHandler::getPrograms() {
    return programs;
}

size_t ShaderHandler::getNumberOfPrograms() {
    return programs.size();
}