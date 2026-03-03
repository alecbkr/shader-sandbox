#pragma once

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
#include "texture/TextureCache.hpp"
#include "core/InspectorEngine.hpp"
#include "core/HotReloader.hpp"
#include "core/FileRegistry.hpp"
#include "presets/PresetAssets.hpp"
#include "core/TextureRegistry.hpp"
#include "core/ui/ConsoleUI.hpp"
#include "core/ui/ViewportUI.hpp"
#include "core/ui/MenuUI.hpp"
#include "core/ui/EditorUI.hpp"
#include "core/ui/InspectorUI.hpp"
#include "application/AppSettings.hpp"
#include "application/Project.hpp"
#include "core/ui/modals/ModalManager.hpp"
#include "core/ui/modals/SettingsModal.hpp"
#include "object/MaterialCache.hpp"

struct AppContext {
    AppContext(const char* _app_title) : app_title(_app_title) {};

    const char* app_title;
    bool shouldClose = false;

    AppSettings settings;
    Project project;

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
    TextureCache texture_cache;
    InspectorEngine inspector_engine;
    HotReloader hot_reloader;
    FileRegistry file_registry;
    ConsoleEngine console_engine;
    EditorEngine editor_engine;
    PresetAssets preset_assets;
    TextureRegistry texture_registry;
    ConsoleUI console_ui;
    ViewportUI viewport_ui;
    MenuUI menu_ui;
    EditorUI editor_ui;
    InspectorUI inspector_ui;
    ModalManager modals;
    SettingsModal settingsModal;
    MaterialCache material_cache;
};
