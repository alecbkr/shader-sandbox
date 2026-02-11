#pragma once

#include <filesystem>
#include <optional>
#include <types.hpp>
#include "core/logging/Logger.hpp"
#include "core/input/ActionRegistry.hpp"
#include "core/input/ContextManager.hpp"
#include "core/input/Keybinds.hpp"
#include "platform/Platform.hpp"
#include "core/input/InputState.hpp"
#include "engine/AppTimer.hpp"
#include "core/EventDispatcher.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/UniformRegistry.hpp"
#include "core/EditorEngine.hpp"
#include "object/ModelCache.hpp"
#include "core/InspectorEngine.hpp"
#include "core/HotReloader.hpp"
#include "core/FileRegistry.hpp"
#include "core/EditorEngine.hpp"
#include "presets/PresetAssets.hpp"
#include "core/TextureRegistry.hpp"
#include "core/ui/ConsoleUI.hpp"
#include "core/ui/ViewportUI.hpp"
#include "core/ui/MenuUI.hpp"
#include "core/ui/EditorUI.hpp"
#include "core/ui/InspectorUI.hpp"
#include "object/ModelImporter.hpp"
#include "persistence/AppSettings.hpp"

struct Project;

struct AppContext {
    AppContext(const char* _app_title) : app_title(_app_title) {};

    u32 width = 960;
    u32 height = 540;
    u32 posX = 100;
    u32 posY = 100;
    const char* app_title;

    std::filesystem::path userConfigDir; // OS config directory (where we store user preferences and settings)
    std::filesystem::path settingsPath; // userConfigDir/settings.json (complete path to the settings)

    //std::optional<Project> project; // loaded project (or none)
    std::filesystem::path projectRoot; // loaded project's root directory
    std::filesystem::path projectShadersDir; // loaded project's shader directory

    Logger logger;
    ActionRegistry action_registry;
    ContextManager ctx_manager;
    Keybinds keybinds;
    Platform platform;
    InputState inputs;
    AppTimer timer;
    EventDispatcher events;
    ShaderRegistry shader_registry;
    UniformRegistry uniform_registry;
    ModelCache model_cache;
    InspectorEngine inspector_engine;
    HotReloader hot_reloader;
    FileRegistry file_registry;
    EditorEngine editor_engine;
    PresetAssets preset_assets;
    TextureRegistry texture_registry;
    ModelImporter model_importer;
    ConsoleUI console_ui;
    ViewportUI viewport_ui;
    MenuUI menu_ui;
    EditorUI editor_ui;
    InspectorUI inspector_ui;
};