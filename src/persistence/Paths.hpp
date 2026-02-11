#pragma once
#include <filesystem>
#include <string>

namespace Paths {
    std::filesystem::path getUserConfigDir(const std::string& appName);
    std::filesystem::path getProjectRootDir(int argc, char** argv, std::string& projectTitle);
}