#pragma once;

#include <filesystem>
#include <types.hpp>

struct AppSettings {
    std::filesystem::path userConfigDir;
    std::filesystem::path settingsPath;

    u32 width = 960;
    u32 height = 540;
    u32 posX = 100;
    u32 posY = 100;
};