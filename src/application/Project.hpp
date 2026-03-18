#pragma once

#include <string>
#include <filesystem>
#include "../core/ConsoleTypes.hpp"

struct Project {
    std::string projectTitle;
    std::filesystem::path projectRoot;
    std::filesystem::path projectShadersDir;
    std::filesystem::path projectJSON;
    ConsoleToggles consoleSettings; 
};