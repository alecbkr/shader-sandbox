#pragma once

#include <nlohmann/json.hpp>

struct Project;

struct UniformPersistence {
    static bool load(Project& project, nlohmann::json& j);
    static bool save(const Project& project, nlohmann::json& j);
};
