#pragma once

#include <string>
#include <filesystem>
#include <vector>

struct Project {
    std::string projectTitle;
    bool previouslySaved = false;
    std::filesystem::path projectRoot;
    std::filesystem::path projectShadersDir;
    std::filesystem::path projectJSON;
    std::vector<std::filesystem::path> openShaderFiles;
};