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
    std::string projectToOpen;

    bool settingsFound = false;

    u32 width = 960;
    u32 height = 540;
    u32 posX = 100;
    u32 posY = 100;

    u8 fontIdx = 3;

    // Keybinds
    std::unordered_map<std::string, SettingsKeybind> keybindsMap = {
        {"cameraForward", {1, {23}, 2, 2}},
        {"cameraBack", {2, {19}, 2, 2}},
        {"cameraLeft", {3, {1}, 2, 2}},
        {"cameraRight", {4, {4}, 2, 2}},
        {"cameraUp", {5, {41}, 2, 2}},
        {"cameraDown", {6, {45}, 2, 2}},
        {"mouseMove", {7, {}, 2, 3}},
        {"switchControlContext", {8, {72}, 3, 0}},
        {"saveActiveShaderFile", {9, {45, 19}, 1, 0}},
        {"saveProject", {10, {47, 19}, 1, 0}},
        {"quitApplication", {11, {47, 74}, 1, 0}},
        {"fontSizeIncrease", {12, {45, 60}, 1, 0}},
        {"fontSizeDecrease", {13, {45, 59}, 1, 0}},
        {"newShaderFile", {14, {45, 14}, 1, 0}},
        {"undo", {15, {45, 26}, 1, 0}},
        {"redo", {16, {45, 25}, 1, 0}},
        {"formatActiveShader", {17, {45, 7}, 1, 0}},
        {"screenshotViewport", {18, {82}, 3, 0}},
        {"fullscreenViewport", {19, {75}, 2, 0}},
        {"editorFind", {20, {45, 6}, 1, 0}},
    };

    // Styles
    SettingsStyles styles;

    // Graphics
    bool vsyncEnabled = false;
};