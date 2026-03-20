#pragma once

#include "core/ShaderRegistry.hpp"
#include <string>
#include <filesystem>
#include "../core/ConsoleTypes.hpp"
#include <vector>

#include "core/EventDispatcher.hpp"
#include "core/UniformRegistry.hpp"

class ShaderRegistry;
class ShaderProgram;

struct Project {
    std::string projectTitle;
    bool previouslySaved = false;
    std::filesystem::path projectRoot;
    std::filesystem::path projectShadersDir;
    std::filesystem::path projectJSON;
    std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> programs;
    std::unordered_map<unsigned int, Uniform> uniforms;
    ShaderRegistry* shaderRegistry = nullptr;
    UniformRegistry* uniformRegistry = nullptr;
    EventDispatcher* events = nullptr;
    ConsoleToggles consoleSettings;
    std::vector<std::filesystem::path> openShaderFiles;
};
