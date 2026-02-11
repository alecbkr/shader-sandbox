#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include "application/AppContext.hpp"

struct AppSettings {
    static int version;

    // To be implemented once projects are fleshed out: std::string lastProject;
    // To be implemented once projects are fleshed out: std::vector<std::string> recentProjects;

    // Load settings from disk, or return defaults if missing
    static bool load(AppContext& ctx);

    // Save settings to disk
    static void save(const AppContext& ctx);
};
