#pragma once

#include <filesystem>
#include <types.hpp>
#include <vector>
#include <unordered_map>
#include "application/SettingsStyles.hpp"

struct SettingsKeybind {
    const u32 action;
    std::vector<u16> keys;
    const u8 context;
    const u8 trigger;
};

struct AppSettings {
    std::filesystem::path userConfigDir;
    std::filesystem::path settingsPath;

    u32 width = 960;
    u32 height = 540;
    u32 posX = 100;
    u32 posY = 100;

    // Keybinds
    std::unordered_map<std::string, SettingsKeybind> keybindsMap = {
        {"cameraForward", {1, {23}, 2, 2}},
        {"cameraBack", {2, {19}, 2, 2}},
        {"cameraLeft", {3, {1}, 2, 2}},
        {"cameraRight", {4, {4}, 2, 2}},
        {"cameraUp", {5, {41}, 2, 2}},
        {"cameraDown", {6, {45}, 2, 2}},
        {"switchControlContext", {7, {72}, 3, 0}},
        {"saveActiveShaderFile", {8, {45, 19}, 1, 0}},
        {"saveProject", {9, {47, 19}, 1, 0}},
        {"quitApplication", {10, {47, 74}, 1, 0}},
    };

    // Styles
    SettingsStyles styles;

    // Graphics
    bool vsyncEnabled = false;
};