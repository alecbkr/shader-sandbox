#pragma once

#include <string>
#include <filesystem>

#include "core/ui/AssetsManager.hpp"

struct Project {
    std::string projectTitle;
    std::filesystem::path projectRoot;
    std::filesystem::path projectShadersDir;
    std::filesystem::path projectJSON;
};